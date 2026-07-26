#pragma once

#include <cstddef>
#include <vector>
#include <memory>
#include <optional>

#include "LightSource.hpp"
#include "Shape.hpp"

#include "BVH.hpp"

class Scene {
private:
    std::vector<std::unique_ptr<Shape>> _shapes;
    std::vector<std::unique_ptr<LightSource>> _lights;
    Color _ambience;

    Node _hierarchy;
    std::vector<size_t> _planeIds;

public:
    Scene(Color ambience = {.05, .05, .05});

    Color Ambience() const;

    void AddShape(std::unique_ptr<Shape> shape);
    void AddLight(std::unique_ptr<LightSource> light);

    std::optional<Intersection> Intersect(const Ray& ray) const;
    
    const std::vector<std::unique_ptr<LightSource>>& Lights() const;
    const std::vector<std::unique_ptr<Shape>>& Shapes() const; 

    ///// BVH
    void BuildHierarchy();
    void BuildNode(
        Node& node,
        std::vector<uint32_t>& ids,
        size_t begin,
        size_t end,
        size_t depth
    );
    void IntersectNode(
        const Node& node,
        const Ray& ray,
        std::optional<Intersection>& closest
    ) const;
};
