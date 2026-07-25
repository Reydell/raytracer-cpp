#include "Vector.hpp"
#include <cmath>
#include <stdexcept>

Vector::Vector()
    : _data{.0f, .0f, .0f}
    {}

Vector::Vector(float x, float y, float z) 
    : _data{x, y, z}
    {}

float& Vector::operator[](size_t ind) {
    return _data[ind];
}
float Vector::operator[](size_t ind) const {
    return _data[ind];
}

void Vector::Normalize() {
    float norm = std::sqrt(
        _data[0] * _data[0] +
        _data[1] * _data[1] +
        _data[2] * _data[2]
    );

    if (norm == .0f) {
        return;
    }

    *this = *this / norm;
}

Vector Vector::Unit() {
    float norm = std::sqrt(
        _data[0] * _data[0] +
        _data[1] * _data[1] +
        _data[2] * _data[2]
    );

    if (norm == .0f) {
        throw std::domain_error{"Zero divison"};
    }

    return *this / norm;
}

float Dot(const Vector& left, const Vector& right) {
    return
        left[0] * right[0] +
        left[1] * right[1] +
        left[2] * right[2];
}
Vector Cross(const Vector& left, const Vector& right) {
    return Vector(
        left[1] * right[2] - left[2] * right[1],
        left[2] * right[0] - left[0] * right[2],
        left[0] * right[1] - left[1] * right[0]
    );
}

Vector operator*(const float left, const Vector& right) {
    return Vector(
        right[0] * left,
        right[1] * left,
        right[2] * left
    );
}
Vector operator*(const Vector& left, const float right) {
    return Vector(
        left[0] * right,
        left[1] * right,
        left[2] * right
    );
}

Vector operator/(const Vector& left, const float right) {
    return Vector(
        left[0] / right,
        left[1] / right,
        left[2] / right
    );
}

Vector Vector::operator+(const Vector& other) const {
    return Vector(
        _data[0] + other[0],
        _data[1] + other[1],
        _data[2] + other[2]
    );
}

Vector& Vector::operator+=(const Vector& other) {
    _data[0] += other[0];
    _data[1] += other[1];
    _data[2] += other[2];
    return *this;
}

Vector Vector::operator-() const {
    return Vector(
        -_data[0],
        -_data[1],
        -_data[2]
    );
}

Vector Vector::operator-(const Vector& other) const {
    return Vector(
        _data[0] - other[0],
        _data[1] - other[1],
        _data[2] - other[2]
    );
}
