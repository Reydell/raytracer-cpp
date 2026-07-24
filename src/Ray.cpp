#include <optional>

#include "Ray.hpp" 
#include "Vector.hpp"
#include "Shape.hpp"

Ray::Ray(Vector origin, Vector direction)
    : _origin{origin},
      _direction{direction} 
{
    _direction.Normalize();    
}

std::optional<Intersection> Ray::Intersect(const Shape& shape) const {
    return shape.Intersect(*this);
}

const Vector& Ray::Direction() const {
    return _direction;
}

const Vector& Ray::Origin() const {
    return _origin;
}