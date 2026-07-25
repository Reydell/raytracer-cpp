#pragma once

#include "Camera.hpp"
#include "Scene.hpp"
#include "Image.hpp"

class RayTracer {
private:
    uint8_t _recursionDepth;

public:
    RayTracer(uint8_t recursionDepth = 2);
    Image Render(const Camera& camera, const Scene& scene) const;

    Color TraceRayRecursive(
        const Ray& ray,
        const Scene& scene,
        uint8_t recursionDepth = 1, 
        bool isInside = false,
        float refractionIndex = 1
    ) const;

    std::optional<Vector> Refract(const Vector& direction, const Vector& normal, float indexFrom, float indexTo) const;
};


