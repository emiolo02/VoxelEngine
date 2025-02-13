#pragma once

#include "Math/Color.hpp"

struct Image {
    std::vector<Color> pixels;
    int32 width = 0, height = 0;
    int32 numChannels = 0;

    uint32 id = 0;
};

class ImageManager {
    SINGLETON(ImageManager)

public:
    const Image &Load(const std::string &path);

    void Save(const Image &image, const char *path) const;

    void Save(uint32 imageId, const char *path) const;

    const Image &GetImage(uint32 imageId);

private:
    std::unordered_map<std::string, Image> m_Images;
    std::unordered_map<uint32, Image *> m_ImagesById;

    uint32 m_NextId = 0;
};
