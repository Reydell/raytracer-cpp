#pragma once

#include <optional>

#include "Shape.hpp"
#include "Vector.hpp"
#include "BVH.hpp"

class Shape;


class Ray {
private:
    Vector _origin;
    Vector _direction;

public:
    Ray(Vector origin, Vector direction);

    const Vector& Direction() const;
    const Vector& Origin() const;

    std::optional<Intersection> Intersect(const Shape& shape) const;
    bool IntersectBox(const Box& box) const;
    
};
