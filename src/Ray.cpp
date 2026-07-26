#include <algorithm>
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

bool Ray::IntersectBox(const Box& box) const {
    std::array<float, 3> entries = {0, 0, 0};
    std::array<float, 3> exits = {0, 0, 0};

    for (size_t i = 0; i < 3; ++i) {
        float entry = (box.min[i] - _origin[i]) / _direction[i];
        float exit = (box.max[i] - _origin[i]) / _direction[i];
        entries[i] = std::min(entry, exit);
        exits[i] = std::max(entry, exit);
    }
    float tEnter = *std::max_element(entries.begin(), entries.end());
    float tExit = *std::min_element(exits.begin(), exits.end());
    
    return ((tEnter <= tExit) && (tExit > 0));
}

const Vector& Ray::Direction() const {
    return _direction;
}

const Vector& Ray::Origin() const {
    return _origin;
}
