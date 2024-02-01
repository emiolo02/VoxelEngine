#pragma once

#include <iostream>

#include "core/gltf.h"
#include "core/app.h"
#include "Model.h"


struct BufferInfo
{
    fx::gltf::Accessor const* Accessor;

    const uint8* Data = nullptr;
    uint32 DataStride = 0;
    uint32 TotalSize = 0;

    bool hasData = false;
};

struct PrimitiveBuffer
{
    BufferInfo positionBuffer;
    BufferInfo normalBuffer;
    BufferInfo tangentBuffer;
    BufferInfo texCoordBuffer;
    BufferInfo indexBuffer;
};

class GLTFinfo 
{
public:
    GLTFinfo(){}

    static std::shared_ptr<Model> LoadGLTF(std::string path);

    static void CalcTangents(Primitive& prim);
private:
    static float ByteToFloat(uint8 b0, uint8 b1, uint8 b2, uint8 b3);

    static Primitive RawToVertex(PrimitiveBuffer primBuffer);


    static uint32_t CalculateDataTypeSize(fx::gltf::Accessor const& accessor);

    static BufferInfo GetData(fx::gltf::Document const& doc, fx::gltf::Accessor const& accessor);

    //PrimitiveBuffer primBuffer;

    std::vector<Primitive> primitives;

    static uint32_t numImg;
};