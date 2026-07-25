#pragma once

#include <array>
#include <cstddef>

class Vector {
private:
    std::array<float, 3> _data;

public:
    Vector();
    Vector(float x, float y, float z);

    float& operator[](size_t ind);
    float operator[](size_t ind) const;

    void Normalize();
    Vector Unit();

    Vector operator+(const Vector& other) const;
    Vector& operator+=(const Vector& other);
    Vector operator-() const;
    Vector operator-(const Vector& other) const;
    
    friend float Dot(const Vector& left, const Vector& right);
    friend Vector Cross(const Vector& left, const Vector& right);

    friend Vector operator*(const float left, const Vector& right);
    friend Vector operator*(const Vector& left, const float right);

    friend Vector operator/(const Vector& left, const float right);


};
