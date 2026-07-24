#include "Scene.hpp"
#include "Shape.hpp"
#include "Ray.hpp"

Scene::Scene(Color ambience) : _ambience(ambience) {}

Color Scene::Ambience() const {
    return _ambience;
}

void Scene::AddShape(std::unique_ptr<Shape> shape) {
    _shapes.push_back(std::move(shape));
}

void Scene::AddLight(std::unique_ptr<LightSource> light) {
    _lights.push_back(std::move(light));
}

std::optional<Intersection> Scene::Intersect(const Ray& ray) const {
    std::optional<Intersection> closestIntersection;

    for (const auto& shape : _shapes) {
        auto intersection = ray.Intersect(*shape);
        
        if (!intersection) {
            continue;
        }

        if (
            !closestIntersection || 
            intersection->distance < closestIntersection->distance
        ) {
            closestIntersection = intersection;    
        }
    }

    return closestIntersection;
}

const std::vector<std::unique_ptr<LightSource>>& Scene::Lights() const {
    return _lights;
}
const std::vector<std::unique_ptr<Shape>>& Scene::Shapes() const {
    return _shapes;
}