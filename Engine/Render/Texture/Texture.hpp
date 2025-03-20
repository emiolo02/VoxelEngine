#pragma once

#include "Math/Color.hpp"

struct Image;

typedef uint32 TextureId;

//------------------------------------------------------------------------------------------

class Texture {
public:
    Texture() = default;

    Texture(Texture &&other) noexcept;

    Texture &operator=(Texture &&other) noexcept;

    Texture(int32 width, int32 height);

    Texture(const uint8 *data, int32 width, int32 height, int32 numChannels);

    ~Texture();

    void BindTexture() const;

    void BindImageTexture() const;

    TextureId GetId() const { return m_Id; }

    int32 GetWidth() const { return m_Width; }
    int32 GetHeight() const { return m_Height; }

private:
    TextureId m_Id = 0;
    int32 m_Width = 0, m_Height = 0, m_NumChannels = 0;
    uint32 m_Target = 0;
};

//------------------------------------------------------------------------------------------

struct Image {
    Image() = default;

    Image(const uint8 *data, int32 width, int32 height, int32 numChannels);

    std::vector<math::Color> pixels;
    int32 width = 0, height = 0, numChannels = 0;
};

//------------------------------------------------------------------------------------------

class TextureManager {
    SINGLETON(TextureManager)

public:
    std::shared_ptr<Texture> Load(const std::string &path, bool reload = false);

    std::shared_ptr<Image> GetTextureImage(const Texture &texture);

    std::shared_ptr<Image> GetTextureImage(TextureId texture);

private:
    std::unordered_map<std::string, std::shared_ptr<Texture> > m_Textures;
    std::unordered_map<TextureId, std::shared_ptr<Image> > m_Images;
};
