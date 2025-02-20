#pragma once

struct Image;
struct Color;

class Texture {
public:
  Texture() = default;

  Texture(Texture &&other) noexcept;

  Texture &operator=(Texture &&other) noexcept;

  Texture(int32 width, int32 height);

  explicit Texture(const Image &image);

  Texture(const Color *colors, int32 width, int32 height, int32 depth, int32 numTextures = 1);

  ~Texture();

  void BindTexture() const;

  void BindImageTexture() const;

  uint32 GetId() const { return m_Id; }

private:
  uint32 m_Id = 0;
  uint32 m_Target = 0;
};

class Texture3D {
public:
  Texture3D() = default;

  Texture3D(const Color *colors, size_t size, size_t numTextures);

  ~Texture3D();

  void BindTexture() const;

  void BindImageTexture() const;

  uint32 GetId() const { return m_Id; }

private:
  uint32 m_Id = 0;
};
