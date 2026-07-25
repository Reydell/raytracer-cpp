#pragma once

#include <cstdint>

struct Pixel;

// all colors will be floats between 0 and 1 and converted at the last moment before drawing
struct Color{
    float r;
    float g;
    float b;

    Pixel ToPixel() const;

    Color operator+(const Color& other) const;
    Color operator*(const Color& other);

    Color& operator+=(const Color& other);
    Color& operator*=(const Color& other);
    Color& operator*=(float scalar);

    friend Color operator*(float left, const Color& right);
    friend Color operator*(const Color& left, float right);
};

struct Material {
    Color emitted;  
    Color diffused;  // coef
    Color specular;  // coef
    Color transparency;  // transparency

    float refractionIndex; // for angles
    Color reflectivity;  // for recursive reflections

    int32_t specExponent;

    Material(
        Color emitted = {0, 0, 0},
        Color diffused = {.4, .4, .6},
        Color specular = {.1, .1, .2},
        Color transparency = {.2, .2, .2},
        float refractionIndex = 1.3f,
        Color reflectivity = {0, 0, 0},
        int32_t specExponent = 500
    );
};
