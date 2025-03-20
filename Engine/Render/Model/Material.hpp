#pragma once

#include "Render/Texture/Texture.hpp"

struct Material {
    vec3 baseColor = {};
    std::shared_ptr<Texture> texture;
};
