#include "LightSource.hpp"
#include "Vector.hpp"
#include "Shape.hpp"
#include "Scene.hpp"
#include "Camera.hpp"
#include "RayTracer.hpp"
#include "Ray.hpp"
#include "Image.hpp"
#include "Material.hpp"
#include "MaterialPresets.hpp"
#include "FileReader.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <vector>


const size_t kWidth = 400;
const size_t kHeight = 800;

const Pixel kBackgroundColor{0, 0, 0};

// no idea how this function works currently
bool UploadPixelArrayToTexture(SDL_Texture *texture, const Image& img, int width, int height) {
  void *textureMemory = nullptr;
  int texturePitch = 0;

  if (!SDL_LockTexture(texture, nullptr, &textureMemory, &texturePitch)) {
    std::cerr << "SDL_LockTexture failed: " << SDL_GetError() << '\n';
    return false;
  }

  auto *destinationBytes = static_cast<std::uint8_t *>(textureMemory);
  const std::size_t bytesPerRow =
      static_cast<std::size_t>(width) * sizeof(Pixel);

  const auto& pixels = img.Pixels();

  for (int y = 0; y < height; ++y) {
    const Pixel *sourceRow =
        pixels.data() +
        static_cast<std::size_t>(y) * static_cast<std::size_t>(width);

    std::uint8_t *destinationRow =
        destinationBytes +
        static_cast<std::size_t>(y) * static_cast<std::size_t>(texturePitch);

    std::memcpy(destinationRow, sourceRow, bytesPerRow);
  }

  SDL_UnlockTexture(texture);
  return true;
}

int main() {
    FileReader reader;
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow(
        "Raytracer", kWidth, kHeight,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );

    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);

    SDL_SetRenderLogicalPresentation(renderer, kWidth, kHeight, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_SetRenderVSync(renderer, 1);

    SDL_Texture *texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        kWidth, kHeight
    );

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    // Actual raytracer part

    RayTracer raytracer{3};
    Camera camera{
        kWidth,
        kHeight,
        Vector{105, 150, 290},
        Vector{-105, -38, -281}
    };
    Scene scene;
    reader.ReadFile(scene, "tests/deer.obj");
    scene.AddLight(std::make_unique<LightSource>(
        Vector{-105, 74, -272},
        Color{1, 1, 1}
    ));

    // Camera camera{
    //     kWidth,
    //     kHeight,
    //     Vector{0, 0, 0},
    //     Vector{0, 0, -1}
    // };
    // Scene scene;
    // reader.ReadFile(scene, "tests/glass_test.obj");

    // std::unique_ptr<Sphere> sph1 = std::make_unique<Sphere>(Vector(0, 0, -6), 2, GLASS);
    // std::unique_ptr<Sphere> sph2 = std::make_unique<Sphere>(Vector(0, 0, -9), 1, CYAN_MATTE);
    // scene.AddShape(std::move(sph1));
    // scene.AddShape(std::move(sph2));

    // std::unique_ptr<LightSource> light1 = std::make_unique<LightSource>(
    //     Vector{0, 0, 10},
    //     Color{1, 1, 1}
    // );
    // std::unique_ptr<LightSource> light2 = std::make_unique<LightSource>(
    //     Vector{0, 8, 6},
    //     Color{1, 1, 1}
    // );
    // std::unique_ptr<LightSource> light3 = std::make_unique<LightSource>(
    //     Vector{-7, 2, 4},
    //     Color{0, 0, 1}
    // );
    // scene.AddLight(std::move(light1));
    // scene.AddLight(std::move(light2));
    // scene.AddLight(std::move(light3));

    bool running = true;

    // loop

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EVENT_QUIT) {
                    running = false;
            }

            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
                running = false;
            }
        }
        
        Image img = raytracer.Render(camera, scene);
        UploadPixelArrayToTexture(texture, img, kWidth, kHeight);


        if (!SDL_SetRenderDrawColor(
            renderer, 0, 0, 0, 255) ||
            !SDL_RenderClear(renderer) ||
            !SDL_RenderTexture(renderer, texture, nullptr, nullptr) ||
            !SDL_RenderPresent(renderer)
        ) {
            std::cerr << "SDL rendering failed: " << SDL_GetError() << '\n';
            break;
        }

    }

    std::cout << '\n';
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
