#include "RayTracer.hpp"
#include "Image.hpp"
#include "Shape.hpp"
#include "Material.hpp"
#include "Scene.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

Image RayTracer::Render(const Camera& camera, const Scene& scene) const {
    size_t width = camera.Width();
    size_t height = camera.Height();
    Image img{width, height};

    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            
            // Color res = this->TraceRay(camera, scene, x, y, 1, false);
            // img.At(x, y) = res.ToPixel();

            Ray ray = camera.CastRay(x, y);
            Color res = this->TraceRayRecursive(ray, scene, 4);
            img.At(x, y) = res.ToPixel();
            
        }
    }

    return img;
}

Color RayTracer::TraceRay(const Camera& camera, const Scene& scene, size_t x, size_t y, [[maybe_unused]] uint8_t depth, [[maybe_unused]] bool isInside) const {
    Ray ray = camera.CastRay(x, y);
    auto intersection = scene.Intersect(ray);

    if (!intersection) {
        return {0, 0, 0};
    }

    // maybe this will go to arguments for recursion
    Color totalLight = {0, 0, 0};

    // ambient
    totalLight += scene.Ambience();


    for (const auto& light : scene.Lights()) { 
        

        // diffused
        Vector viewLight = (light->position - intersection->point).Unit();
        float dotNormViewlight = std::max(0.f, Dot(intersection->normal, viewLight));
        Color diffused = intersection->material.diffused * light->color * dotNormViewlight;

        totalLight += diffused;

        //specular
        Vector viewReflect = 2 * dotNormViewlight * intersection->normal - viewLight;
        Vector viewEye = (ray.Origin() - intersection->point).Unit();

        float dotReflectEye = std::max(0.f, Dot(viewReflect, viewEye));

        totalLight += intersection->material.specular * light->color * std::pow(dotReflectEye, intersection->material.specExponent);
    
        // emitted
        totalLight += intersection->material.emitted;


    }

    return totalLight;  //  stub
}

Color RayTracer::TraceRayRecursive(
    const Ray& ray,
    const Scene& scene,
    uint8_t recursionDepth,
    bool isInside,
    Color refractionIndex
) const {

    auto intersection = scene.Intersect(ray);

    if (!intersection) {
        return {0, 0, 0};
    }

    if (recursionDepth == 0) {
        return scene.Ambience() + intersection->material.emitted;
    }

    Color totalLight = {0, 0, 0};

    Vector viewEye = -ray.Direction();

    for (const auto& light : scene.Lights()) { 

        // diffused
        Vector viewLight = (light->position - intersection->point).Unit();
        float dotNormViewlight = std::max(0.f, Dot(intersection->normal, viewLight));
        Color diffused = intersection->material.diffused * light->color * dotNormViewlight;

        totalLight += diffused;

        //specular
        Vector viewReflect = 2 * dotNormViewlight * intersection->normal - viewLight;

        float dotReflectEye = std::max(0.f, Dot(viewReflect, viewEye));

        totalLight += intersection->material.specular * light->color * std::pow(dotReflectEye, intersection->material.specExponent);
    }

    float dotNormVieweye = std::max(0.f, Dot(intersection->normal, viewEye));
    Vector dirReflected = 2 * dotNormVieweye * intersection->normal - viewEye;

    if (!isInside) {
        Color reflected = TraceRayRecursive(
            Ray{intersection->point + 1e-4f * dirReflected, dirReflected}, 
            scene,
            recursionDepth - 1,
            isInside,
            intersection->material.refractionCoef
        );

        totalLight += intersection->material.reflectivity * reflected;
    }

    Color refracted = {0, 0, 0};

    return totalLight + intersection->material.transparency * refracted;
}
