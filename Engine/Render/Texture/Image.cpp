#include "Image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void
ImageManager::Save(const Image &image, const char *path) const {
    if (stbi_write_png(path, image.width, image.height, 4, image.pixels.data(), image.width * 4)) {
        std::cout << "Wrote image to \"" << path << "\"\n";
        return;
    }
    std::cout << "Failed to write image \"" << path << "\"\n";
}

void
ImageManager::Save(const uint32 imageId, const char *path) const {
    const Image &image = *m_ImagesById.at(imageId);
    stbi_flip_vertically_on_write(true);
    std::vector<math::Color> toWrite(image.pixels.size());
    for (int i = 0; i < image.pixels.size(); ++i) {
        toWrite[i] = {
            image.pixels[i].a,
            image.pixels[i].b,
            image.pixels[i].g,
            image.pixels[i].r
        };
    }
    if (stbi_write_png(path, image.width, image.height, 4, toWrite.data(), image.width * 4)) {
        std::cout << "Wrote image to \"" << path << "\"\n";
        return;
    }
    std::cout << "Failed to write image \"" << path << "\"\n";
}


const Image &
ImageManager::GetImage(const uint32 imageId) {
    return *m_ImagesById[imageId];
}

const Image &
ImageManager::Load(const std::string &path) {
    if (m_Images.contains(path)) {
        return m_Images[path];
    }

    //stbi_set_flip_vertically_on_load(true);

    int width, height, numChannels;
    uint8 *data = stbi_load(path.c_str(), &width, &height, &numChannels, 0);

    if (!data) {
        Image &image = m_Images[path] = {
                           {math::Color(0xFFFFFFFF)},
                           1, 1, 4,
                           m_NextId++
                       };
        m_ImagesById[image.id] = &image;
        return image;
    }

    Image &image = m_Images[path] = {
                       std::vector<math::Color>(width * height),
                       width, height, numChannels,
                       m_NextId++
                   };

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const int index = x + y * width;
            const uint8 alpha = numChannels == 4 ? data[index * numChannels + 3] : 255;

            image.pixels[index] = {
                data[index * numChannels + 0],
                data[index * numChannels + 1],
                data[index * numChannels + 2],
                alpha
            };
            //std::cout <<
            //        (int) data[index * numChannels + 0] << ' ' <<
            //        (int) data[index * numChannels + 1] << ' ' <<
            //        (int) data[index * numChannels + 2] << ' ' <<
            //        (int) alpha << '\n';
        }
    }

    stbi_image_free(data);

    m_ImagesById[image.id] = &image;

    std::cout << "Loaded image \"" << path << "\"\n";

    return image;
}
