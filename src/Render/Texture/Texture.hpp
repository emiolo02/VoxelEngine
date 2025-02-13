#pragma once

struct Image;

class Texture {
public:
  Texture() = default;

  Texture(int32 width, int32 height);

  explicit Texture(const Image &image);

  ~Texture();

  void BindTexture() const;

  void BindImageTexture() const;

  uint32 GetId() const { return m_Id; }

  int32 GetWidth() const { return m_Width; }
  int32 GetHeight() const { return m_Height; }

private:
  uint32 m_Id = 0;
  int32 m_Width = 0;
  int32 m_Height = 0;
};
