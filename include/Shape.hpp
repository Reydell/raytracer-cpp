#pragma once

#include <array>
#include <optional>
#include <cstddef>

#include "Material.hpp"
#include "Vector.hpp"
#include "BVH.hpp"

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
    virtual std::optional<Box> GetBox() const;

protected:
    explicit Shape(const Material& material) : _material(material) {}
    virtual void CalculateBox() = 0;
    Box _box;

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

protected:
    void CalculateBox() override;
};

class Triangle : public Shape {
private:
    std::array<Vector, 3> _vertices;
    Vector _edgeFirst;
    Vector _edgeSecond;
    Vector _normal;
public:
    Triangle(const Vector& first, const Vector& second, const Vector& third, const Material& material);

    const Vector& operator[](size_t ind) const;

    std::optional<Intersection> Intersect(const Ray& ray) const override;

protected:
    void CalculateBox() override;
};

class Plane : public Shape {
private:
    Vector _origin;
    Vector _normal;
public:
    Plane(const Vector& origin, const Vector& normal, const Material& material);

    std::optional<Intersection> Intersect(const Ray& ray) const override;

    std::optional<Box> GetBox() const override;

protected:
    void CalculateBox() override;
    
};