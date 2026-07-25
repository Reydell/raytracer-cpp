#include <cmath>

#include "Camera.hpp"
#include "Vector.hpp"

Camera::Camera(float width, float height, Vector position, Vector forward, Vector up, float verticalFov)
    : _position{position},
      _verticalFov{verticalFov},
      _width{width},
      _height{height}
{   
    _aspect = width / height;
    _horizontalFov = 2.f * std::atan2(
        std::tan(_verticalFov / 2.f) * _aspect,
        1.f
    );

    _forward = forward.Unit();
    _up = up.Unit();
    _right = Cross(forward, up).Unit();  // .Unit() just in case

    _stepRight = _right * (2 * std::tan(_horizontalFov / 2) / _width);
    _stepUp = _up * (2 * std::tan(_verticalFov / 2) / _height);

    _zeroPixelCenter = _position + _forward 
        - _stepRight * (static_cast<float>(_width - 1) / 2) 
        + _stepUp * (static_cast<float>(_height - 1) / 2);
}

Ray Camera::CastRay(int x, int y) const {
    Vector pixelCenter = _zeroPixelCenter + static_cast<float>(x) * _stepRight - static_cast<float>(y) * _stepUp;
    Ray ray{_position, pixelCenter - _position};
    return ray;
}

size_t Camera::Width() const {
    return _width;
}
size_t Camera::Height() const {
    return _height;
}

Vector Camera::Position() const {
    return _position;
}

void Camera::Rotate(const Vector& axis, float angle) {
    // make sure when passing an argument that axis is normalized
    Vector axisAbs = axis[0] * _right + axis[1] * _up + axis[2] * _forward;

    float cos = std::cos(angle);
    float sin = std::sin(angle);
    
    // rotate _forward
    Vector axisCrossForward = Cross(axisAbs, _forward);
    Vector projForward = Dot(axisAbs, _forward) * axisAbs;
    Vector rejForward = _forward - projForward;

    _forward = projForward + cos * rejForward + sin * axisCrossForward;

    // rotate _up
    Vector axisCrossUp = Cross(axisAbs, _up);
    Vector projUp = Dot(axisAbs, _up) * axisAbs;
    Vector rejUp = _up - projUp;

    _up = projUp + cos * rejUp + sin * axisCrossUp;

    // other calculations
    _right = Cross(_forward, _up);

    _stepRight = _right * (2 * std::tan(_horizontalFov / 2) / _width);
    _stepUp = _up * (2 * std::tan(_verticalFov / 2) / _height);

    _zeroPixelCenter = _position + _forward 
        - _stepRight * (static_cast<float>(_width - 1) / 2) 
        + _stepUp * (static_cast<float>(_height - 1) / 2);
}

// void Camera::LookAt(const Vector& point) {

// }

void Camera::Move(const Vector& offset) {
    Vector offsetAbs = offset[0] * _right + offset[1] * _up + offset[2] * _forward;
    _position = _position + offsetAbs;
    _zeroPixelCenter = _position + _forward 
        - _stepRight * (static_cast<float>(_width - 1) / 2) 
        + _stepUp * (static_cast<float>(_height - 1) / 2);
}
