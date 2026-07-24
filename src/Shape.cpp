#include "Shape.hpp"
#include "Material.hpp"
#include "Ray.hpp"
#include <cmath>
#include <limits>
#include <optional>

Material Shape::GetMaterial() const {
    return _material;
}

Sphere::Sphere(const Vector& center, float radius, const Material& material) 
    : Shape(material),
      _radius{radius},
      _center{center}
{}

std::optional<Intersection> Sphere::Intersect(const Ray& ray) const {
    float b = 2 * Dot(ray.Direction(), ray.Origin() - _center);
    float c = Dot(ray.Origin() - _center, ray.Origin() - _center) - _radius * _radius;

    float discriminant = b * b - 4 * c;

    if (discriminant < 0) {
        return std::nullopt;
    }

    // optimize number of calculations later
    float solnSmaller = (-b - std::sqrt(discriminant)) / 2;
    if (solnSmaller >= 0) {
        Vector point = ray.Origin() + solnSmaller * ray.Direction();
        return Intersection{
            solnSmaller,
            point,
            (point - _center).Unit(),
            this->GetMaterial()
        };
    }

    float solnLarger = (-b + std::sqrt(discriminant)) / 2;
    if (solnLarger >= 0) {
        Vector point = ray.Origin() + solnLarger * ray.Direction();
        return Intersection{
            solnLarger,
            point,
            (point - _center).Unit(),
            this->GetMaterial()
        };
    }

    return std::nullopt;
}
