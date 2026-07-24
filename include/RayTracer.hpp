#pragma once

#include "Camera.hpp"
#include "Scene.hpp"
#include "Image.hpp"

class RayTracer {
public:
    Image Render(const Camera& camera, const Scene& scene) const;

    Color TraceRay(const Camera& camera, const Scene& scene, size_t x, size_t y, [[maybe_unused]] uint8_t depth, [[maybe_unused]] bool isInside) const;
    Color TraceRayRecursive(
        const Ray& ray,
        const Scene& scene,
        uint8_t recursionDepth = 1, 
        bool isInside = false,
        Color refractionIndex = {1, 1, 1}
    ) const;
};
