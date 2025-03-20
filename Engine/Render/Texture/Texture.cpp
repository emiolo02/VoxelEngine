#include "Texture.hpp"

#include "GL/glew.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(Texture &&other) noexcept {
    m_Id = other.m_Id;
    m_Target = other.m_Target;

    other.m_Id = 0;
    other.m_Target = 0;
}

//------------------------------------------------------------------------------------------

Texture &
Texture::operator=(Texture &&other) noexcept {
    if (this != &other) {
        m_Id = other.m_Id;
        m_Target = other.m_Target;

        other.m_Id = 0;
        other.m_Target = 0;
    }

    return *this;
}

//------------------------------------------------------------------------------------------

Texture::Texture(const int32 width, const int32 height) {
    glGenTextures(1, &m_Id);
    glActiveTexture(GL_TEXTURE0);
    m_Target = GL_TEXTURE_2D;
    glBindTexture(GL_TEXTURE_2D, m_Id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA,
                 GL_FLOAT, nullptr);
}

//------------------------------------------------------------------------------------------

Texture::Texture(const uint8 *data,
                 const int32 width,
                 const int32 height,
                 const int32 numChannels)
    : m_Width(width),
      m_Height(height),
      m_NumChannels(numChannels),
      m_Target(GL_TEXTURE_2D) {
    glGenTextures(1, &m_Id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(m_Target, m_Id);
    glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    const GLenum format = numChannels == 4 ? GL_RGBA : GL_RGB;
    glTexImage2D(m_Target, 0, GL_RGBA32F, width, height, 0, format, GL_UNSIGNED_INT, data);
}

//------------------------------------------------------------------------------------------

Texture::~Texture() {
    if (m_Id != 0)
        glDeleteTextures(1, &m_Id);
}

//------------------------------------------------------------------------------------------

void
Texture::BindTexture() const {
    glBindTexture(m_Target, m_Id);
}

//------------------------------------------------------------------------------------------

void
Texture::BindImageTexture() const {
    glBindImageTexture(0, m_Id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

//------------------------------------------------------------------------------------------

Image::Image(const uint8 *data,
             const int32 width,
             const int32 height,
             const int32 numChannels)
    : width(width),
      height(height),
      numChannels(numChannels) {
    pixels.resize(width * height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const int index = x + y * width;
            const uint8 alpha = numChannels == 4 ? data[index * numChannels + 3] : 255;

            pixels[index] = math::Color(
                data[index * numChannels + 0],
                data[index * numChannels + 1],
                data[index * numChannels + 2],
                alpha
            );
        }
    }
}

//------------------------------------------------------------------------------------------

std::shared_ptr<Texture>
TextureManager::Load(const std::string &path, const bool reload) {
    if (m_Textures.contains(path) && !reload) {
        return m_Textures[path];
    }

    int32 w, h, n;
    uint8 *data = stbi_load(path.c_str(), &w, &h, &n, 0);
    assert(data);

    m_Textures[path] = std::make_shared<Texture>(data, w, h, n);
    m_Images[m_Textures[path]->GetId()] = std::make_shared<Image>(data, w, h, n);
    stbi_image_free(data);

    return m_Textures[path];
}

//------------------------------------------------------------------------------------------

std::shared_ptr<Image>
TextureManager::GetTextureImage(const Texture &texture) {
    if (m_Images.contains(texture.GetId())) {
        return m_Images[texture.GetId()];
    }

    return nullptr;
}

//------------------------------------------------------------------------------------------

std::shared_ptr<Image>
TextureManager::GetTextureImage(const TextureId texture) {
    if (m_Images.contains(texture)) {
        return m_Images[texture];
    }

    return nullptr;
}
