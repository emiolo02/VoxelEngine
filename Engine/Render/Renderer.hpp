#pragma once

#include "DataStructures/BrickMap.hpp"
#include "Shader/Shader.hpp"
#include "Shader/StorageBuffer.hpp"
#include "Texture/Texture.hpp"


class Renderer {
public:
    Renderer();

    void Render() const;

    void SetDimensions(int32 width, int32 height);

    const Texture &GetRenderTexture() const { return m_RenderTexture; }

    void SetBrickMap(BrickMap *brickMap) { m_BrickMap = brickMap; }

    StorageBuffer<uint32> &GetBrickGridBuffer() { return m_BrickGridBuffer; }
    StorageBuffer<BrickMap::Brick> &GetSolidMaskBuffer() { return m_SolidMaskBuffer; }
    StorageBuffer<BrickMap::BrickTexture> &GetBrickTextureBuffer() { return m_BrickTextureBuffer; }

private:
    void Blit() const;

    int32 m_Width = 0, m_Height = 0;
    Texture m_RenderTexture;

    BrickMap *m_BrickMap = nullptr;

    StorageBuffer<uint32> m_BrickGridBuffer;
    StorageBuffer<BrickMap::Brick> m_SolidMaskBuffer;
    StorageBuffer<BrickMap::BrickTexture> m_BrickTextureBuffer;

    Shader m_RaytraceBrickmap;
    Shader m_Blit;
};
