#include "Texture.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

#include "Image.hpp"

Texture::Texture(Texture &&other) noexcept {
    m_Id = other.m_Id;
    m_Target = other.m_Target;

    other.m_Id = 0;
    other.m_Target = 0;
}

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

Texture::Texture(const Image &image) {
    glGenTextures(1, &m_Id);
    m_Target = GL_TEXTURE_2D;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_Id);

    glTexParameteri(m_Target, GL_TEXTURE_SWIZZLE_R, GL_ALPHA);
    glTexParameteri(m_Target, GL_TEXTURE_SWIZZLE_G, GL_BLUE);
    glTexParameteri(m_Target, GL_TEXTURE_SWIZZLE_B, GL_GREEN);
    glTexParameteri(m_Target, GL_TEXTURE_SWIZZLE_A, GL_RED);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, image.width, image.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, image.pixels.data());
}

Texture::Texture(const Color *colors,
                 const int32 width,
                 const int32 height,
                 const int32 depth,
                 const int32 numTextures) {
    glGenTextures(1, &m_Id);

    std::cout << "Current: " << depth * numTextures << '\n';
    std::cout << "Max: " << GL_MAX_ARRAY_TEXTURE_LAYERS << '\n';
    m_Target = numTextures == 1 ? GL_TEXTURE_2D : GL_TEXTURE_2D_ARRAY;
    glBindTexture(m_Target, m_Id);
    //glTexStorage3D(m_Target, 1, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, width, height, depth * numTextures);
    std::cout << "TexStorage\n";

    //glTexSubImage3D(m_Target, 0, 0, 0, 0, width, height, depth * numTextures, GL_RGBA, GL_UNSIGNED_BYTE, colors);
    std::cout << "TexSubImage\n";

    glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage3D(m_Target, 0, GL_COMPRESSED_RGB, width, height, depth * numTextures, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 colors);
}

Texture::~Texture() {
    if (m_Id != 0)
        glDeleteTextures(1, &m_Id);
}

void
Texture::BindTexture() const {
    glBindTexture(m_Target, m_Id);
}

void
Texture::BindImageTexture() const {
    glBindImageTexture(0, m_Id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}
