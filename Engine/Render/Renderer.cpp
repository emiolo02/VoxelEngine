#include "Renderer.hpp"

#include "Camera.hpp"
#include "GL/glew.h"

#include "Vertex/VertexArray.hpp"
#include "Vertex/IndexBuffer.hpp"

#include "Display/Window.hpp"

constexpr float quadVertices[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f
};
constexpr uint32 quadIndices[] = {
    0, 1, 2,
    2, 3, 0
};

//------------------------------------------------------------------------------------------

struct Quad {
    VertexArray vertexArray;
    VertexBuffer vertexBuffer;
    IndexBuffer indexBuffer;

    Quad()
        : vertexBuffer(quadVertices, 12 * sizeof(float)),
          indexBuffer(quadIndices, 6) {
        VertexBufferLayout layout;
        layout.Push<float>(3);
        vertexArray.AddBuffer(vertexBuffer, layout);
    }
};

//------------------------------------------------------------------------------------------

Renderer::Renderer()
    : m_BrickGridBuffer(2),
      m_SolidMaskBuffer(3),
      m_BrickTextureBuffer(4),
      m_RaytraceBrickmap("shaders/rtBrickmap.comp"),
      m_Blit("shaders/fullscreen.vert", "shaders/blit.frag") {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

//------------------------------------------------------------------------------------------

void
Renderer::Render() const {
    if (m_Width <= 0 || m_Height <= 0) {
        std::cerr << "Invalid renderer dimensions.\n"
                << "\tWidth: " << m_Width << '\n'
                << "\tHeight: " << m_Height << '\n';
        return;
    }

    if (m_BrickMap == nullptr) {
        std::cerr << "Renderer has no reference to a brickmap.\n";
        return;
    }

    const Camera *mainCamera = Camera::GetMainCamera();

    if (mainCamera == nullptr) {
        std::cerr << "Main camera is not set.\n";
        return;
    }


    m_RenderTexture.BindImageTexture();

    m_RaytraceBrickmap.Bind();

    m_BrickGridBuffer.Bind();
    m_SolidMaskBuffer.Bind();
    m_BrickTextureBuffer.Bind();

    m_RaytraceBrickmap.SetValue("u_CameraPosition", mainCamera->GetPosition());
    m_RaytraceBrickmap.SetValue("u_InvProjection", mainCamera->GetInvProjection());
    m_RaytraceBrickmap.SetValue("u_InvView", mainCamera->GetInvView());

    const BoundingBox &boundingBox = m_BrickMap->GetBoundingBox();

    m_RaytraceBrickmap.SetValue("u_GridMinBounds", boundingBox.min);
    m_RaytraceBrickmap.SetValue("u_GridMaxBounds", boundingBox.max);
    m_RaytraceBrickmap.SetValue("u_VoxelSize", m_BrickMap->GetVoxelSize());

    const ivec3 &dimensions = m_BrickMap->GetDimensions();

    m_RaytraceBrickmap.SetValue("u_GridXSize", dimensions.x);
    m_RaytraceBrickmap.SetValue("u_GridYSize", dimensions.y);
    m_RaytraceBrickmap.SetValue("u_GridZSize", dimensions.z);
    m_RaytraceBrickmap.SetValue("u_Resolution", vec2(m_Width, m_Height));

    m_RaytraceBrickmap.SetValue("u_ShowSteps", false);
    m_RaytraceBrickmap.SetValue("u_ShowNormals", false);


    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glDispatchCompute(
        std::ceil(static_cast<float>(m_Width) / 16.0f),
        std::ceil(static_cast<float>(m_Height) / 16.0f),
        1
    );
    Blit();
}

//------------------------------------------------------------------------------------------

void
Renderer::SetDimensions(const int32 width, const int32 height) {
    if (m_Width == width && m_Height == height) {
        // No need to recreate render texture if dimensions are the same.
        return;
    }
    m_Width = width, m_Height = height;
    m_RenderTexture = std::move(Texture(m_Width, m_Height));
}

//------------------------------------------------------------------------------------------

void
Renderer::Blit() const {
    static Quad fullscreenQuad;

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    m_Blit.Bind();

    m_RenderTexture.BindTexture();

    fullscreenQuad.vertexBuffer.Bind();
    fullscreenQuad.vertexArray.Bind();
    fullscreenQuad.indexBuffer.Bind();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
