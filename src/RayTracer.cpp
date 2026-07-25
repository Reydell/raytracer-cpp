#include "RayTracer.hpp"
#include "Image.hpp"
#include "Shape.hpp"
#include "Material.hpp"
#include "Scene.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

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

    using Clock = std::chrono::steady_clock;
    const auto renderStart = Clock::now();
    auto lastProgressUpdate = renderStart;
    constexpr size_t progressBarWidth = 40;

    std::cout << "\rRendering [" << std::string(progressBarWidth, '-')
              << "]   0% | 0.0s elapsed | -- remaining"
              << std::flush;

    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            
            // Color res = this->TraceRay(camera, scene, x, y, 1, false);
            // img.At(x, y) = res.ToPixel();

            Ray ray = camera.CastRay(x, y);
            Color res = this->TraceRayRecursive(ray, scene, _recursionDepth);
            img.At(x, y) = res.ToPixel();
            
        }

        const size_t completedColumns = x + 1;
        const auto now = Clock::now();
        const bool renderFinished = completedColumns == width;

        if (renderFinished ||
            now - lastProgressUpdate >= std::chrono::milliseconds{100}) {
            const double progress =
                static_cast<double>(completedColumns) /
                static_cast<double>(width);
            const double elapsed =
                std::chrono::duration<double>(now - renderStart).count();
            const double remaining = elapsed * (1.0 - progress) / progress;
            const size_t filled =
                static_cast<size_t>(progress * progressBarWidth);

            std::ostringstream line;
            line << "\rRendering ["
                 << std::string(filled, '#')
                 << std::string(progressBarWidth - filled, '-')
                 << "] "
                 << std::setw(3) << static_cast<int>(progress * 100.0)
                 << "% | "
                 << std::fixed << std::setprecision(1)
                 << elapsed << "s elapsed | "
                 << remaining << "s remaining"
                 << "\033[K";

            std::cout << line.str() << std::flush;
            lastProgressUpdate = now;
        }
    }

    std::cout << '\n';
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
        totalLight += scene.Ambience();
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
    if (!isInside) {
        Color reflected = TraceRayRecursive(
            Ray{intersection->point + 1e-4f * dirReflected, dirReflected}, 
            scene,
            recursionDepth - 1,
            isInside,
            refractionIndex
        );

        totalLight += intersection->material.reflectivity * reflected;
    }

    // refracted ray calculation

    std::optional<Vector> opt_dirRefracted = Refract(ray.Direction(), intersection->normal, refractionIndex, intersection->material.refractionIndex);
    if (!opt_dirRefracted) {
        return totalLight;
    }
    Vector dirRefracted = *opt_dirRefracted;
    
    Color refracted = TraceRayRecursive(
        Ray(intersection->point + 1e-4f * dirRefracted, dirRefracted),
        scene,
        recursionDepth -1,
        !isInside,
        intersection->material.refractionIndex
    );


    return totalLight + (isInside ? Color{1., 1., 1.} : intersection->material.transparency) * refracted;
}
