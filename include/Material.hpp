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
    Color ambient; // same as diffused for now
    Color specular;  // coef
    Color tint; // for light passing through this

    float refractionIndex; // for angles
    
    float transparency; // Tr = 1 - d
    float reflectivity;

    float specExponent;

    Material(
        Color emitted = {0, 0, 0},
        Color diffused = {.5, .5, .5},
        Color specular = {.1, .1, .2},
        Color tint = {1, 1, 1}, 
        float refractionIndex = 1.3f,
        float transparency = 0,
        float reflectivity = 0,
        float specExponent = 32
    );
};
