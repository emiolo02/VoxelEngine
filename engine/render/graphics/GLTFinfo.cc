#include "config.h"
#include "GLTFinfo.h"
#include "TextureResource.h"

uint32_t GLTFinfo::numImg;


std::shared_ptr<Model>
GLTFinfo::LoadGLTF(std::string path)
{
    
    std::vector<std::string> out;
    split(path, '.', out);

    fx::gltf::Document doc;
    if (out.back() == "gltf")
        doc = fx::gltf::LoadFromText(path);
    else if (out.back() == "glb")
        doc = fx::gltf::LoadFromBinary(path);
    else
        assert(false); //what


    // Load texture information
    std::vector<ImageInfo> imageInfos;

    for (auto& image : doc.images)
    {
        ImageInfo imInfo;
        
        if (!image.uri.empty() && !image.IsEmbeddedResource())
        {
            imInfo.FilePath = (fx::gltf::detail::GetDocumentRootPath(path) / image.uri).string();
            imInfo.name = image.uri;
            imageInfos.push_back(imInfo);
        }
        else if (image.IsEmbeddedResource())
        {
            std::vector<uint8_t> embeddedData;
            image.MaterializeData(embeddedData);
            
            imInfo.name = "embeddedImage_" + std::to_string(numImg);
            numImg++;
            imInfo.BinaryData = embeddedData;
            imInfo.isBinary = true;
            imageInfos.push_back(imInfo);
        }
        else
        {
            fx::gltf::BufferView const& bufferView = doc.bufferViews[image.bufferView];
            fx::gltf::Buffer const& buffer = doc.buffers[bufferView.buffer];

            imInfo.name = "embeddedImage_" + std::to_string(numImg);
            numImg++;

            for (int i = bufferView.byteOffset; i < bufferView.byteOffset + bufferView.byteLength; i++)
            {
                imInfo.BinaryData.push_back(buffer.data[i]);
            }
            imInfo.isBinary = true;
            imageInfos.push_back(imInfo);
        }
    }

    // Load vertex data

    std::vector<Mesh> loadedMeshes;

    for (auto& mesh : doc.meshes)
    {
        std::vector<Primitive> loadedPrimitives;

        for (auto& primitive : mesh.primitives)
        {
            PrimitiveBuffer primBuffer;

            for (auto& attrib : primitive.attributes)
            {
                // Get vertex attributes
                if (attrib.first == "POSITION")
                {
                    primBuffer.positionBuffer = GetData(doc, doc.accessors[attrib.second]);
                    primBuffer.positionBuffer.hasData = true;
                }
                else if (attrib.first == "NORMAL")
                {
                    primBuffer.normalBuffer = GetData(doc, doc.accessors[attrib.second]);
                    primBuffer.normalBuffer.hasData = true;
                }
                else if (attrib.first == "TANGENT")
                {
                    primBuffer.tangentBuffer = GetData(doc, doc.accessors[attrib.second]);
                    primBuffer.tangentBuffer.hasData = true;
                }
                else if (attrib.first == "TEXCOORD_0")
                {
                    primBuffer.texCoordBuffer = GetData(doc, doc.accessors[attrib.second]);
                    primBuffer.texCoordBuffer.hasData = true;
                }
            }
            primBuffer.indexBuffer = GetData(doc, doc.accessors[primitive.indices]);

            Primitive prim = RawToVertex(primBuffer);

            // Load textures

            if (primitive.material > -1)
            {
                prim.material = new BlinnPhongMaterial();
                fx::gltf::Material mat = doc.materials[primitive.material];
                
                if (mat.pbrMetallicRoughness.baseColorTexture.index > -1)
                {
                    int texIndex = doc.textures[mat.pbrMetallicRoughness.baseColorTexture.index].source;
                    ImageInfo& im = imageInfos[texIndex];
                    TextureData texData = TextureResource::Instance()->LoadTexture(im, false);
                    texData.type = COLOR;
                    prim.material->AddTexture(Texture(texData));
                }
                else
                    prim.material->AddTexture(Texture(TextureResource::Instance()->GetDefaultWhite()));

                
                if (mat.normalTexture.index > -1)
                {
                    int texIndex = doc.textures[mat.normalTexture.index].source;
                    ImageInfo& im = imageInfos[texIndex];
                    TextureData texData = TextureResource::Instance()->LoadTexture(im, false);
                    texData.type = NORMAL;
                    prim.material->AddTexture(Texture(texData));
                }
                else
                {
                    TextureData texData = TextureResource::Instance()->GetDefaultNormal();
                    texData.type = NORMAL;
                    prim.material->AddTexture(Texture(texData));
                }

                
                if (mat.emissiveTexture.index > -1)
                {
                    int texIndex = doc.textures[mat.emissiveTexture.index].source;
                    ImageInfo& im = imageInfos[texIndex];
                    TextureData texData = TextureResource::Instance()->LoadTexture(im, false);
                    texData.type = EMMISIVE;
                    prim.material->AddTexture(Texture(texData));
                }
                else
                {
                    TextureData texData = TextureResource::Instance()->GetDefaultBlack();
                    texData.type = EMMISIVE;
                    prim.material->AddTexture(Texture(texData));
                }

                
                if (mat.occlusionTexture.index > -1)
                {
                    int texIndex = doc.textures[mat.occlusionTexture.index].source;
                    ImageInfo& im = imageInfos[texIndex];
                    TextureData texData = TextureResource::Instance()->LoadTexture(im, false);
                    texData.type = OCCLUSION;
                    prim.material->AddTexture(Texture(texData));
                }
                else
                {
                    TextureData texData = TextureResource::Instance()->GetDefaultWhite();
                    texData.type = OCCLUSION;
                    prim.material->AddTexture(Texture(texData));
                }
            
                
                if (mat.pbrMetallicRoughness.metallicRoughnessTexture.index > -1)
                {
                    int texIndex = doc.textures[mat.pbrMetallicRoughness.metallicRoughnessTexture.index].source;
                    ImageInfo& im = imageInfos[texIndex];
                    TextureData texData = TextureResource::Instance()->LoadTexture(im, false);
                    texData.type = METALLIC_ROUGHNESS;
                    prim.material->AddTexture(Texture(texData));
                }
                else
                {
                    TextureData texData = TextureResource::Instance()->GetDefaultBlack();
                    texData.type = METALLIC_ROUGHNESS;
                    prim.material->AddTexture(Texture(texData));
                }
                
            }


            loadedPrimitives.push_back(prim);
        }
        loadedMeshes.push_back(loadedPrimitives);
    }

    return std::make_shared<Model>(loadedMeshes);
}

float
GLTFinfo::ByteToFloat(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3)
{
    float f;
    unsigned char b[] = { b0, b1, b2, b3 };
    memcpy(&f, &b, sizeof(f));
    return f;
}

Primitive 
GLTFinfo::RawToVertex(PrimitiveBuffer primBuffer)
{
    // Convert raw byte buffers to vertex information
    Primitive prim;

    for (uint32 i = 0; i < primBuffer.indexBuffer.TotalSize; i += primBuffer.indexBuffer.DataStride)
    {
        std::vector<uint8> binData;
        for (int j = 0; j < primBuffer.indexBuffer.DataStride; j++)
            binData.push_back(primBuffer.indexBuffer.Data[i + j]);

        uint32 index = 0;
        std::memcpy(&index, binData.data(), primBuffer.indexBuffer.DataStride);
        prim.indices.push_back(index);
        //prim.indices.push_back((primBuffer.indexBuffer.Data[i * 2 + 1] << 8) | primBuffer.indexBuffer.Data[i * 2]);
    }

    std::vector<vec3> positions;
    std::vector<vec3> normals;
    std::vector<vec2> UVs;
    std::vector<vec4> tangents;

    if (primBuffer.positionBuffer.hasData)
        for (uint32 i = 0; i < primBuffer.positionBuffer.TotalSize; i += 12)
        {
            vec3 pos;
            for (int j = 0; j < 12; j += 4)
            {
                uint8_t b[4] { 0 };

                for (int k = 0; k < 4; k++)
                    b[k] = primBuffer.positionBuffer.Data[i + j + k];

                pos[j / 4] = ByteToFloat(b[0], b[1], b[2], b[3]);
            }

            positions.push_back(pos);
        }

    if (primBuffer.normalBuffer.hasData)
        for (uint32 i = 0; i < primBuffer.normalBuffer.TotalSize; i += 12)
        {
            vec3 nor;
            for (int j = 0; j < 12; j += 4)
            {
                uint8_t b[4] { 0 };

                for (int k = 0; k < 4; k++)
                    b[k] = primBuffer.normalBuffer.Data[i + j + k];

                nor[j / 4] = ByteToFloat(b[0], b[1], b[2], b[3]);
            }

            normals.push_back(nor);
        }

    if (primBuffer.texCoordBuffer.hasData)
        for (uint32 i = 0; i < primBuffer.texCoordBuffer.TotalSize; i += 8)
        {
            vec2 UV;
            for (int j = 0; j < 8; j += 4)
            {
                uint8_t b[4] { 0 };

                for (int k = 0; k < 4; k++)
                    b[k] = primBuffer.texCoordBuffer.Data[i + j + k];

                UV[j / 4] = ByteToFloat(b[0], b[1], b[2], b[3]);
            }

            UVs.push_back(UV);
        }

    tangents.resize(positions.size());
    if (primBuffer.tangentBuffer.hasData)
    {
        tangents.resize(0);
        for (uint32 i = 0; i < primBuffer.tangentBuffer.TotalSize; i += 16)
        {
            vec4 tang;
            for (int j = 0; j < 16; j += 4)
            {
                uint8_t b[4] { 0 };

                for (int k = 0; k < 4; k++)
                    b[k] = primBuffer.tangentBuffer.Data[i + j + k];

                tang[j / 4] = ByteToFloat(b[0], b[1], b[2], b[3]);
            }

            tangents.push_back(tang);
        }
    }
    

    std::vector<Vertex> vertices;
    for (int i = 0; i < positions.size(); i++)
    {
        vertices.push_back
        (
            Vertex
            (
                positions[i],
                UVs[i],
                normals[i],
                tangents[i]
            )
        );
    }

    prim.vertices = vertices;

    if (!primBuffer.tangentBuffer.hasData)
    {
        CalcTangents(prim);
    }

    return prim;
}

void
GLTFinfo::CalcTangents(Primitive& prim)
{
    // calculate tangent xyz

    for (int i = 0; i < prim.vertices.size(); i += 3)
    {
        int i0 = prim.indices[i];
        int i1 = prim.indices[i + 1];
        int i2 = prim.indices[i + 2];

        vec3 pos0 = prim.vertices[i0].position;
        vec3 pos1 = prim.vertices[i1].position;
        vec3 pos2 = prim.vertices[i2].position;

        vec2 tex0 = prim.vertices[i0].UV;
        vec2 tex1 = prim.vertices[i1].UV;
        vec2 tex2 = prim.vertices[i2].UV;

        vec3 edge1 = pos1 - pos0;
        vec3 edge2 = pos2 - pos0;

        vec2 uv1 = tex1 - tex0;
        vec2 uv2 = tex2 - tex0;

        float r = 1.0f / (uv1.x * uv2.y - uv1.y * uv2.x);

        vec4 tangent(
            ((edge1.x * uv2.y) - (edge2.x * uv1.y)) * r,
            ((edge1.y * uv2.y) - (edge2.y * uv1.y)) * r,
            ((edge1.z * uv2.y) - (edge2.z * uv1.y)) * r,
            1
        );

        prim.vertices[i0].tangent += tangent;
        prim.vertices[i1].tangent += tangent;
        prim.vertices[i2].tangent += tangent;
    }
}

uint32_t 
GLTFinfo::CalculateDataTypeSize(fx::gltf::Accessor const& accessor)
{
    uint32_t elementSize = 0;
    switch (accessor.componentType)
    {
    case fx::gltf::Accessor::ComponentType::Byte:
    case fx::gltf::Accessor::ComponentType::UnsignedByte:
        elementSize = 1;
        break;
    case fx::gltf::Accessor::ComponentType::Short:
    case fx::gltf::Accessor::ComponentType::UnsignedShort:
        elementSize = 2;
        break;
    case fx::gltf::Accessor::ComponentType::Float:
    case fx::gltf::Accessor::ComponentType::UnsignedInt:
        elementSize = 4;
        break;
    }

    switch (accessor.type)
    {
    case fx::gltf::Accessor::Type::Mat2:
        return 4 * elementSize;
        break;
    case fx::gltf::Accessor::Type::Mat3:
        return 9 * elementSize;
        break;
    case fx::gltf::Accessor::Type::Mat4:
        return 16 * elementSize;
        break;
    case fx::gltf::Accessor::Type::Scalar:
        return elementSize;
        break;
    case fx::gltf::Accessor::Type::Vec2:
        return 2 * elementSize;
        break;
    case fx::gltf::Accessor::Type::Vec3:
        return 3 * elementSize;
        break;
    case fx::gltf::Accessor::Type::Vec4:
        return 4 * elementSize;
        break;
    }

    return 0;
}

BufferInfo 
GLTFinfo::GetData(fx::gltf::Document const& doc, fx::gltf::Accessor const& accessor)
{
    fx::gltf::BufferView const& bufferView = doc.bufferViews[accessor.bufferView];
    fx::gltf::Buffer const& buffer = doc.buffers[bufferView.buffer];

    const uint32_t dataTypeSize = CalculateDataTypeSize(accessor);
    return BufferInfo{ &accessor, &buffer.data[(uint64_t)bufferView.byteOffset + accessor.byteOffset], dataTypeSize, accessor.count * dataTypeSize };
}