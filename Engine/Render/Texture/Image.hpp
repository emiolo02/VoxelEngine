#pragma once

#include "Math/Color.hpp"

struct Image {
    std::vector<math::Color> pixels;
    int32 width = 0, height = 0;
    int32 numChannels = 0;

    int32 id = -1;
};

class ImageManager {
    SINGLETON(ImageManager)

public:
    const Image &Load(const std::string &path);

    void Save(const Image &image, const char *path) const;

    void Save(int32 imageId, const char *path) const;

    const Image &GetImage(int32 imageId);

private:
    std::unordered_map<std::string, Image> m_Images;
    std::unordered_map<int32, Image *> m_ImagesById;

    int32 m_NextId = 0;
};
