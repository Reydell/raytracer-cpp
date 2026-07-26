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
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <system_error>
#include <vector>
#include <chrono>


const size_t kWidth = 1200;
const size_t kHeight = 800;
const float kMovementSpeed = .1;
const float kRotationSpeed = .0008;

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

int main(int argc, char* argv[]) {
    FileReader reader;
    std::string sceneFolder = "ballcube";
    std::uint8_t recursionDepth = 1;

    for (int i = 1; i < argc; ++i) {
        const std::string argument = argv[i];
        if (argument != "-r") {
            sceneFolder = argument;
            continue;
        }

        if (++i >= argc) {
            std::cerr << "Missing recursion depth after -r\n";
            return 1;
        }

        unsigned int parsedDepth = 0;
        const std::string depthArgument = argv[i];
        const auto [end, error] = std::from_chars(
            depthArgument.data(),
            depthArgument.data() + depthArgument.size(),
            parsedDepth
        );
        if (
            error != std::errc{} ||
            end != depthArgument.data() + depthArgument.size() ||
            parsedDepth == 0 ||
            parsedDepth > std::numeric_limits<std::uint8_t>::max()
        ) {
            std::cerr << "Recursion depth must be an integer from 1 to 255\n";
            return 1;
        }

        recursionDepth = static_cast<std::uint8_t>(parsedDepth);
    }

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow(
        "Raytracer", kWidth, kHeight,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );

    SDL_SetWindowRelativeMouseMode(window, true);

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

    RayTracer raytracer{recursionDepth};
    // Camera camera{
    //     kWidth,
    //     kHeight,
    //     Vector{105, 150, 290},
    //     Vector{-105, -38, -281}
    // };
    // Scene scene;
    // reader.ReadFile(scene, "deer");
    Camera camera{
        kWidth,
        kHeight,
        Vector{1.5, 1.5, -0.1},
        Vector{0, 0, -1}    
    };
    Scene scene;
    reader.ReadFile(scene, sceneFolder);
    scene.BuildHierarchy();

    bool running = true;
    Uint64 previousTicks = SDL_GetTicks();
    bool needsRender = true;

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

        const bool *keyboard = SDL_GetKeyboardState(nullptr);

        Vector offset{0, 0, 0};
        if (keyboard[SDL_SCANCODE_A]) {
            offset += Vector{-1, 0, 0};
            needsRender = true;
        }
        if (keyboard[SDL_SCANCODE_D]) {
            offset += Vector{1, 0, 0};
            needsRender = true;
        }
        if (keyboard[SDL_SCANCODE_W]) {
            offset += Vector{0, 0, 1};
            needsRender = true;
        }
        if (keyboard[SDL_SCANCODE_S]) {
            offset += Vector{0, 0, -1};
            needsRender = true;
        }
        if (keyboard[SDL_SCANCODE_LCTRL]) {
            offset += Vector{0, -1, 0};
            needsRender = true;
        }
        if (keyboard[SDL_SCANCODE_SPACE]) {
            offset += Vector{0, 1, 0};
            needsRender = true;
        }
        offset.Normalize();
        camera.Move(offset * kMovementSpeed);

        float mouseDeltaX = .0f;
        float mouseDeltaY = .0f;
        SDL_MouseButtonFlags mouseBtns = SDL_GetRelativeMouseState(&mouseDeltaX, &mouseDeltaY);
        if (mouseBtns & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) {
            Vector rotationAxisY = Vector(0, 1, 0) * -mouseDeltaX;
            Vector rotationAxisX = Vector(1, 0, 0) * -mouseDeltaY;

            if (mouseDeltaX or mouseDeltaY) {
                needsRender = true;
                camera.Rotate(rotationAxisY, kRotationSpeed);
                camera.Rotate(rotationAxisX, kRotationSpeed);
            }
            
        }
        if (needsRender) {
            const auto start = std::chrono::steady_clock::now();
            Image img = raytracer.RenderMT(camera, scene);
            const auto end = std::chrono::steady_clock::now();
            const std::chrono::duration<float> elapsed = end - start;
            std::cout << "\rRender time: " << elapsed.count()  << " seconds" << std::flush;

            UploadPixelArrayToTexture(texture, img, kWidth, kHeight);
            needsRender = false;
        }
        


        if (!SDL_SetRenderDrawColor(
            renderer, 0, 0, 0, 255) ||
            !SDL_RenderClear(renderer) ||
            !SDL_RenderTexture(renderer, texture, nullptr, nullptr) ||
            !SDL_RenderPresent(renderer)
        ) {
            std::cerr << "SDL rendering failed: " << SDL_GetError() << '\n';
            break;
        }

        //camera movement
        // camera.Move(Vector(0, 0, .01));

    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
