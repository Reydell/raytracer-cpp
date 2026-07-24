#pragma once

#include "Material.hpp"
#include "Vector.hpp"

struct LightSource {
    Vector position;
    Color color;

    LightSource(const Vector& position = Vector(), const Color& color = {1.f, 1.f, 1.f});
};