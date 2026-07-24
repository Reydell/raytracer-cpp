#pragma once

#include <array>
#include <optional>
#include <cstddef>

#include "Material.hpp"
#include "Vector.hpp"

class Ray;

struct Intersection {
    float distance;
    Vector point;
    Vector normal;
    Material material;
};

class Shape {
public:
    virtual std::optional<Intersection> Intersect(const Ray& ray) const = 0;
    virtual ~Shape() = default;

    Material GetMaterial() const;

protected:
    explicit Shape(const Material& material) : _material(material) {}

private:
    Material _material;
};

class Sphere : public Shape {
private:
    float _radius;
    Vector _center;
public:
    Sphere(const Vector& center, float radius, const Material& material);

    std::optional<Intersection> Intersect(const Ray& ray) const override;
};

class Triangle : public Shape {
private:
    std::array<Vector, 3> _vertices;
public:
    Triangle(const Vector& first, const Vector& second, const Vector& third, const Material& material);

    Vector operator[](size_t ind) const;

    std::optional<Intersection> Intersect(const Ray& ray) const override;
};

class Plane : public Shape {
private:
    Vector _origin;
    Vector _normal;
public:
    Plane(const Vector& origin, const Vector& normal, const Material& material);

    std::optional<Intersection> Intersect(const Ray& ray) const override;
};