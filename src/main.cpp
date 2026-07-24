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


const size_t kWidth = 800;
const size_t kHeight = 600;
const int kFpsSampleFrames = 20;

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

    RayTracer raytracer;
    Camera camera{kWidth, kHeight};

    Scene scene;
    std::unique_ptr<Sphere> sph1 = std::make_unique<Sphere>(Vector(0, 0, -6), 1, DIM_LIGHTBULB);
    std::unique_ptr<Sphere> sph2 = std::make_unique<Sphere>(Vector(2, 2, -8), 1, YELLOW_GLOSSY);
    std::unique_ptr<Sphere> sph3 = std::make_unique<Sphere>(Vector(-2, -1, -4), 1, CYAN_MATTE);
    scene.AddShape(std::move(sph1));
    scene.AddShape(std::move(sph2));
    scene.AddShape(std::move(sph3));

    std::unique_ptr<LightSource> light1 = std::make_unique<LightSource>(
        Vector{6, 0, 0},
        Color{1, 0, 0}
    );
    std::unique_ptr<LightSource> light2 = std::make_unique<LightSource>(
        Vector{0, 5, 3},
        Color{0, 1, 0}
    );
    std::unique_ptr<LightSource> light3 = std::make_unique<LightSource>(
        Vector{-7, 2, 4},
        Color{0, 0, 1}
    );
    scene.AddLight(std::move(light1));
    scene.AddLight(std::move(light2));
    scene.AddLight(std::move(light3));

    bool running = true;
    Uint64 fpsTimer = SDL_GetTicksNS();
    int fpsFrameCount = 0;

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

        if (++fpsFrameCount == kFpsSampleFrames) {
            const Uint64 now = SDL_GetTicksNS();
            std::cout << "\033[31mFPS: "
                      << kFpsSampleFrames * 1e9 / static_cast<double>(now - fpsTimer)
                      << "\033[0m\n";
            fpsTimer = now;
            fpsFrameCount = 0;
        }

    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
