#include "Material.hpp"
#include "Image.hpp"

#include <cstdint>
#include <cmath>
#include <algorithm>

Pixel Color::ToPixel() const {
    return Pixel(
        static_cast<std::uint8_t>(std::lround(std::clamp(r * 255, .0f, 255.f))),
        static_cast<std::uint8_t>(std::lround(std::clamp(g * 255, .0f, 255.f))),
        static_cast<std::uint8_t>(std::lround(std::clamp(b * 255, .0f, 255.f)))
    );
}

bool Color::IsZero() {
    return !static_cast<bool>(r || b || g); 
}

Color Color::operator+(const Color& other) const {
    return {
        r + other.r,
        g + other.g,
        b + other.b
    };
}

Color Color::operator*(const Color& other) {
    return {
        r * other.r,
        g * other.g,
        b * other.b
    };
}

Color& Color::operator+=(const Color& other) {
    r += other.r;
    g += other.g;
    b += other.b;
    return *this;
}

Color& Color::operator*=(const Color& other) {
    r *= other.r;
    g *= other.g;
    b *= other.b;
    return *this;
}

Color& Color::operator*=(float scalar) {
    r *= scalar;
    g *= scalar;
    b *= scalar;
    return *this;
}

Color operator*(float left, const Color& right) {
    return {
        left * right.r,
        left * right.g,
        left * right.b
    };
}

Color operator*(const Color& left, float right) {
    return {
        left.r * right,
        left.g * right,
        left.b * right
    };
}

Material::Material(
    Color emitted,
    Color ambient,
    Color diffused,
    Color specular,
    Color tint,
    float refractionIndex,
    float transparency,
    float reflectivity,
    float lightsConstant,
    float specExponent
) : emitted(emitted),
    diffused(diffused),
    ambient(ambient),
    specular(specular),
    tint(tint),
    refractionIndex(refractionIndex),
    transparency(transparency),
    lightsConstant(lightsConstant),
    specExponent(specExponent) 
{
    this->reflectivity = std::min(reflectivity, 1 - transparency);
}
