#include <cmath>

#include "Camera.hpp"

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

int Camera::Width() const {
    return _width;
}
int Camera::Height() const {
    return _height;
}

Vector Camera::Position() const {
    return _position;
}

void Rotate(const Vector& axis) {
    //stub
    return;
}

void Camera::Move(const Vector& offset) {
    _position = _position + offset;
    _zeroPixelCenter = _position + _forward 
        - _stepRight * (static_cast<float>(_width - 1) / 2) 
        + _stepUp * (static_cast<float>(_height - 1) / 2);
}
