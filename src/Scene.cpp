#include "Scene.hpp"
#include "Shape.hpp"
#include "Ray.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include "BVH.hpp"

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

    // No BVH (OLD)
    // for (const auto& shape : _shapes) {
    //     auto intersection = ray.Intersect(*shape);
        
    //     if (!intersection) {
    //         continue;
    //     }

    //     if (
    //         !closestIntersection || 
    //         intersection->distance < closestIntersection->distance
    //     ) {
    //         closestIntersection = intersection;    
    //     }
    // }

    for (size_t id : _planeIds) {
        auto intersection = _shapes[id]->Intersect(ray);
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

    IntersectNode(_hierarchy, ray, closestIntersection);

    return closestIntersection;
}

const std::vector<std::unique_ptr<LightSource>>& Scene::Lights() const {
    return _lights;
}

const std::vector<std::unique_ptr<Shape>>& Scene::Shapes() const {
    return _shapes;
}

void Scene::BuildHierarchy() {
    std::vector<uint32_t> ids;
    for (size_t id = 0; id < _shapes.size(); ++id) {
        if (_shapes[id]->GetBox()) {
            ids.push_back(static_cast<uint32_t>(id));
        } else {
            _planeIds.push_back(id);
        }
    }

    BuildNode(_hierarchy, ids, 0, ids.size(), 0);
}

void Scene::BuildNode(
        Node& node,
        std::vector<uint32_t>& ids,
        size_t begin,
        size_t end,
        size_t depth
    ) {
    const size_t count = end - begin;

    // box union
    bool noBoxYet = true;
    for (size_t i = begin; i < end; ++i) {
        auto box = _shapes[ids[i]]->GetBox();
        if (!box) {
            continue;
        }

        if (noBoxYet) {
            node.box = *box;
            noBoxYet = false;
        } else {
            node.box |= *box;
        }
    }

    if (count <= 3) {
        node.objectIds.assign(
            ids.begin() + begin,
            ids.begin() + end
        );
        return;
    }

    const size_t axis = depth % 3;
    const size_t middle = begin + count / 2;

    std::nth_element(
        ids.begin() + begin,
        ids.begin() + middle,
        ids.begin() + end,
        [this, axis](uint32_t leftId, uint32_t rightId) {
            return _shapes[leftId]->GetBox()->centroid[axis] <
                   _shapes[rightId]->GetBox()->centroid[axis];
        }
    );

    node.left = std::make_unique<Node>();
    node.right = std::make_unique<Node>();

    BuildNode(*node.left, ids, begin, middle, depth + 1);
    BuildNode(*node.right, ids, middle, end, depth + 1);
}

void Scene::IntersectNode(
        const Node& node,
        const Ray& ray,
        std::optional<Intersection>& closest
    ) const {
    if (!ray.IntersectBox(node.box)) {
        return;
    }

    if (node.IsLeaf()) {
        for (size_t id: node.objectIds) {
            auto optIntersection = _shapes[id]->Intersect(ray);
            if (
                optIntersection &&
                (!closest || optIntersection->distance < closest->distance)
            ) {
                closest = optIntersection;
            }
        }
        return;
    }
    if (node.left) {
        IntersectNode(*node.left, ray, closest);
    }
    if (node.right) {    
        IntersectNode(*node.right, ray, closest);
    }
}