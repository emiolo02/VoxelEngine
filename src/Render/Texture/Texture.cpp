#include "Texture.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

#include "Image.hpp"

Texture::Texture(const int32 width, const int32 height)
    : m_Width(width), m_Height(height) {
    glGenTextures(1, &m_Id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_Id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA,
                 GL_FLOAT, nullptr);
}

Texture::Texture(const Image &image)
    : m_Width(image.width), m_Height(image.height) {
    glGenTextures(1, &m_Id);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_Id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, image.width, image.height, 0, GL_RGBA,
                 GL_UNSIGNED_INT, image.pixels.data());
}

Texture::~Texture() {
    if (m_Id != 0)
        glDeleteTextures(1, &m_Id);
}

void
Texture::BindTexture() const {
    glBindTexture(GL_TEXTURE_2D, m_Id);
}

void
Texture::BindImageTexture() const {
    glBindImageTexture(0, m_Id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}
