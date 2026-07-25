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

    bool IsZero();
};

struct Material {
    Color emitted;  
    Color diffused;  // coef
    Color ambient; // same as diffused for now
    Color specular;  // coef
    Color tint; // for light passing through this

    float refractionIndex; // for angles
    
    float transparency; // Tr = 1 - d al2
    float reflectivity; // al1
    float lightsConstant; // al0

    float specExponent;

    Material(
        Color emitted = {0, 0, 0},
        Color ambient = {0, 0, 0},
        Color diffused = {0, 0, 0},
        Color specular = {0, 0, 0},
        Color tint = {1, 1, 1}, 
        float refractionIndex = 1.3f,
        float transparency = 0,
        float reflectivity = 0,
        float lightsConstant = 1,
        float specExponent = 300
    );
};
