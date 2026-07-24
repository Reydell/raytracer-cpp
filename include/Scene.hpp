#pragma once

#include <vector>
#include <memory>
#include <optional>

#include "LightSource.hpp"
#include "Shape.hpp"

class Scene {
private:
    std::vector<std::unique_ptr<Shape>> _shapes;
    std::vector<std::unique_ptr<LightSource>> _lights;
    Color _ambience;
public:
    Scene(Color ambience = {.05, .05, .05});

    Color Ambience() const;

    void AddShape(std::unique_ptr<Shape> shape);
    void AddLight(std::unique_ptr<LightSource> light);

    std::optional<Intersection> Intersect(const Ray& ray) const;
    
    const std::vector<std::unique_ptr<LightSource>>& Lights() const;
    const std::vector<std::unique_ptr<Shape>>& Shapes() const; 
};