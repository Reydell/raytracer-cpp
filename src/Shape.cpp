#include "Shape.hpp"
#include "BVH.hpp"
#include "Material.hpp"
#include "Ray.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>

Material Shape::GetMaterial() const {
    return _material;
}

std::optional<Box> Shape::GetBox() const {
    return _box;
}

Sphere::Sphere(const Vector& center, float radius, const Material& material) 
    : Shape(material),
      _radius{radius},
      _center{center}
{
    CalculateBox();
}

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

void Sphere::CalculateBox() {
    _box = {
        _center - Vector{_radius, _radius, _radius},
        _center + Vector{_radius, _radius, _radius},
        _center
    };
}

Triangle::Triangle(
    const Vector& first,
    const Vector& second,
    const Vector& third,
    const Material& material
)
    : Shape(material),
      _vertices{first, second, third}
{
    _edgeFirst = _vertices[1] - _vertices[0];
    _edgeSecond = _vertices[2] - _vertices[0];
    _normal = Cross(_edgeFirst, _edgeSecond).Unit();
    CalculateBox();
}

const Vector& Triangle::operator[](size_t ind) const {
    return _vertices[ind];
}

std::optional<Intersection> Triangle::Intersect(const Ray& ray) const {

    const Vector directionCrossEdge = Cross(ray.Direction(), _edgeSecond);
    const float determinant = Dot(_edgeFirst, directionCrossEdge);

    if (std::abs(determinant) < 1e-4f) {
        return std::nullopt;
    }

    const float inverseDeterminant = 1.f / determinant;
    const Vector originOffset = ray.Origin() - _vertices[0];
    const float firstBarycentric =
        Dot(originOffset, directionCrossEdge) * inverseDeterminant;

    if (firstBarycentric < 0.f || firstBarycentric > 1.f) {
        return std::nullopt;
    }

    const Vector originCrossEdge = Cross(originOffset, _edgeFirst);
    const float secondBarycentric =
        Dot(ray.Direction(), originCrossEdge) * inverseDeterminant;

    if (secondBarycentric < 0.f ||
        firstBarycentric + secondBarycentric > 1.f) {
        return std::nullopt;
    }

    const float distance =
        Dot(_edgeSecond, originCrossEdge) * inverseDeterminant;

    if (distance < 0.f) {
        return std::nullopt;
    }

    Vector normal = _normal;
    if (Dot(normal, ray.Direction()) > 0.f) {
        normal = -normal;
    }

    return Intersection{
        distance,
        ray.Origin() + distance * ray.Direction(),
        normal,
        GetMaterial()
    };
}

void Triangle::CalculateBox() {
    const auto [minX, maxX] = std::minmax({_vertices[0][0], _vertices[1][0], _vertices[2][0]});
    const auto [minY, maxY] = std::minmax({_vertices[0][1], _vertices[1][1], _vertices[2][1]});
    const auto [minZ, maxZ] = std::minmax({_vertices[0][2], _vertices[1][2], _vertices[2][2]});

    _box = {
        {minX, minY, minZ},
        {maxX, maxY, maxZ},
        (_vertices[0] + _vertices[1] + _vertices[2]) / 3.f
    };
}

Plane::Plane(const Vector& origin, const Vector& normal, const Material& material) 
    : Shape(material),
      _origin(origin),
      _normal(normal)
{
    _normal.Normalize();
}

std::optional<Intersection> Plane::Intersect(const Ray& ray) const {
    const float denominator = Dot(ray.Direction(), _normal);
    if (std::abs(denominator) < 1e-6f) {
        return std::nullopt;
    }

    const float distance = Dot(_origin - ray.Origin(), _normal) / denominator;
    if (distance < 1e-4f) {
        return std::nullopt;
    }

    return Intersection{
        distance,
        ray.Origin() + distance * ray.Direction(),
        Dot(ray.Direction(), _normal) < 0 ? _normal : -_normal,
        GetMaterial()
    };
}

std::optional<Box> Plane::GetBox() const {
    return std::nullopt;
}

void Plane::CalculateBox() {
    return;  // just overrides a virtual method of Shape
}
