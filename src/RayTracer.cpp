#include "RayTracer.hpp"
#include "Image.hpp"
#include "Shape.hpp"
#include "Material.hpp"
#include "Scene.hpp"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <optional>
#include <system_error>
#include <thread>

RayTracer::RayTracer(uint8_t recursionDepth) : _recursionDepth(recursionDepth) {}

std::optional<Vector> RayTracer::Refract(const Vector& direction, const Vector& normal, float indexFrom, float indexTo) const {
    Vector correctedNormal = (Dot(direction, normal)) < 0 ? normal : -normal;
    float eta = indexFrom / indexTo;

    float incidentCos = - Dot(direction, correctedNormal);
    float refractionDiscr = 1 - eta * eta * (1 - incidentCos * incidentCos);

    if (refractionDiscr < 0) {
        return std::nullopt;
    }

    return eta * direction + (eta * incidentCos - std::sqrt(refractionDiscr)) * correctedNormal;

}

Image RayTracer::Render(const Camera& camera, const Scene& scene) const {
    size_t width = camera.Width();
    size_t height = camera.Height();
    Image img{width, height};

    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            
            // Color res = this->TraceRay(camera, scene, x, y, 1, false);
            // img.At(x, y) = res.ToPixel();

            Ray ray = camera.CastRay(x, y);
            Color res = this->TraceRayRecursive(ray, scene, _recursionDepth);
            img.At(x, y) = res.ToPixel();
            
        }
    }

    return img;
}


Color RayTracer::TraceRayRecursive(
    const Ray& ray,
    const Scene& scene,
    uint8_t recursionDepth,
    bool isInside,
    float refractionIndex
) const {

    auto intersection = scene.Intersect(ray);

    if (!intersection) {
        return {0, 0, 0};
    }

    Color totalLight = {0, 0, 0};

    if (recursionDepth == _recursionDepth) {
        totalLight += scene.Ambience() * intersection->material.ambient;
    }
    // emitted
    totalLight += intersection->material.emitted;

    Vector viewEye = -ray.Direction();
    
    for (const auto& light : scene.Lights()) { 
        // diffused
        Vector viewLight = (light->position - intersection->point).Unit();
        float dotNormViewlight = Dot(intersection->normal, viewLight);

        if (dotNormViewlight <= 0.f) {
            continue;
        }



        Color diffused = intersection->material.diffused * light->color * dotNormViewlight;

        totalLight += diffused;

        //specular
        Vector viewReflect = 2 * dotNormViewlight * intersection->normal - viewLight;

        float dotReflectEye = std::max(0.f, Dot(viewReflect, viewEye));

        totalLight += intersection->material.specular * light->color * std::pow(dotReflectEye, intersection->material.specExponent);
    }

    if (recursionDepth == 1) {
        return totalLight;
    }

    float dotNormVieweye = std::max(0.f, Dot(intersection->normal, viewEye));
    Vector dirReflected = 2 * dotNormVieweye * intersection->normal - viewEye;

    // reflected ray calculation
    if (!isInside && !(intersection->material.specular.IsZero())) {
        Color reflected = TraceRayRecursive(
            Ray{intersection->point + 1e-4f * dirReflected, dirReflected}, 
            scene,
            recursionDepth - 1,
            isInside,
            refractionIndex
        );

        totalLight += intersection->material.specular * reflected;
    }

    // refracted ray calculation

    if (intersection->material.transparency == 0) {
        return totalLight;
    }

    float nextRefractionIndex = isInside ? 1.0f : intersection->material.refractionIndex;

    std::optional<Vector> opt_dirRefracted = Refract(ray.Direction(), intersection->normal, refractionIndex, nextRefractionIndex);
    if (!opt_dirRefracted) {
        return totalLight;
    }
    Vector dirRefracted = *opt_dirRefracted;
    
    Color refracted = TraceRayRecursive(
        Ray(intersection->point + 1e-4f * dirRefracted, dirRefracted),
        scene,
        recursionDepth -1,
        !isInside,
        nextRefractionIndex
    );


    return totalLight + intersection->material.transparency * (isInside ? Color{1., 1., 1.} : intersection->material.tint) * refracted;
}


void RayTracer::RenderWorker(
    const Camera& camera,
    const Scene& scene,
    Image& img,
    std::atomic<size_t>& nextCol
) const {
    const size_t width = camera.Width();
    const size_t height = camera.Height();

    while (true) {
        const size_t x = nextCol.fetch_add(1u, std::memory_order_relaxed);
        if (x >= width) {
            break;
        }
        for (size_t y = 0; y < height; ++y) {
            Ray ray = camera.CastRay(x, y);
            Color res = this->TraceRayRecursive(ray, scene, _recursionDepth);
            img.At(x, y) = res.ToPixel();
        }
    }
}

Image  RayTracer::RenderMT(const Camera& camera, const Scene& scene) const {
    size_t width = camera.Width();
    size_t height = camera.Height();
    Image img{width, height};
    size_t workerCount = std::max(1u, std::thread::hardware_concurrency());
    // workerCount = std::min(width, workerCount); 

    std::atomic<size_t> nextCol{0};

    std::vector<std::jthread> workers;
    workers.reserve(workerCount);

    for (size_t i = 0; i < workerCount; ++i) {
        workers.emplace_back([&, this] {
            RenderWorker(camera, scene, img, nextCol);
        });
    }

    for (std::jthread& worker : workers) {
        worker.join();
    }

    return img;   
}
