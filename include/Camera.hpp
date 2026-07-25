#pragma once

#include <cmath>
#include <numbers>

#include "Vector.hpp"
#include "Ray.hpp"

class Camera {
private:
    Vector _position;
    Vector _forward;
    Vector _up;
    Vector _right;
    float _verticalFov;

    float _horizontalFov;
    float _aspect;
    float _width;
    float _height;

    Vector _stepRight;
    Vector _stepUp;
    Vector _zeroPixelCenter;

public:
    Camera(
        float width,  // values taken from Image class
        float height,
        Vector position = Vector(), 
        Vector forward = Vector(0, 0, -1),  // same as vector from camera to screen center
        Vector up = Vector(0, 1, 0), 
        float verticalFov = std::numbers::pi_v<float> / 3
    );

    Ray CastRay(int x, int y) const;  // camera will not have its own width and height

    size_t Width() const;
    size_t Height() const;

    Vector Position() const;

    void Rotate(const Vector& axis, float angle);  // rotate relative to camera view
    void LookAt(const Vector& point); // stub
    void Move(const Vector& offset); // move relative to camera view
    void MoveTo(const Vector& point); // stub
};