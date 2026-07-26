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


constexpr int kDefaultWidth = 800;
constexpr int kDefaultHeight = 600;
const float kMovementSpeed = 1.;
const float kRotationSpeed = .5;

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
    int width = kDefaultWidth;
    int height = kDefaultHeight;

    for (int i = 1; i < argc; ++i) {
        const std::string argument = argv[i];
        if (argument != "-r" && argument != "-w" && argument != "-h") {
            if (!argument.empty() && argument.front() == '-') {
                std::cerr << "Unknown option: " << argument << '\n';
                return 1;
            }
            sceneFolder = argument;
            continue;
        }

        if (++i >= argc) {
            std::cerr << "Missing value after " << argument << '\n';
            return 1;
        }

        unsigned int parsedValue = 0;
        const std::string valueArgument = argv[i];
        const auto [end, error] = std::from_chars(
            valueArgument.data(),
            valueArgument.data() + valueArgument.size(),
            parsedValue
        );
        if (
            error != std::errc{} ||
            end != valueArgument.data() + valueArgument.size() ||
            parsedValue == 0
        ) {
            std::cerr << "Value after " << argument << " must be a positive integer\n";
            return 1;
        }

        if (argument == "-r") {
            if (parsedValue > std::numeric_limits<std::uint8_t>::max()) {
                std::cerr << "Recursion depth must be an integer from 1 to 255\n";
                return 1;
            }
            recursionDepth = static_cast<std::uint8_t>(parsedValue);
            continue;
        }

        if (parsedValue > static_cast<unsigned int>(std::numeric_limits<int>::max())) {
            std::cerr << "Image dimensions are too large\n";
            return 1;
        }

        if (argument == "-w") {
            width = static_cast<int>(parsedValue);
        } else {
            height = static_cast<int>(parsedValue);
        }
    }

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow(
        "Raytracer", width, height,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );

    SDL_SetWindowRelativeMouseMode(window, true);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);

    SDL_SetRenderLogicalPresentation(renderer, width, height, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_SetRenderVSync(renderer, 1);

    SDL_Texture *texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        width, height
    );

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    // Actual raytracer part

    RayTracer raytracer{recursionDepth};
    // Camera camera{
    //     width,
    //     height,
    //     Vector{105, 150, 290},
    //     Vector{-105, -38, -281}
    // };
    // Scene scene;
    // reader.ReadFile(scene, "deer");
    Camera camera{
        static_cast<float>(width),
        static_cast<float>(height),
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

        const Uint64 currentTicks = SDL_GetTicks();
        float deltaSeconds = static_cast<float>(currentTicks - previousTicks) / 1000.f;
        previousTicks = currentTicks;
        deltaSeconds = std::min(deltaSeconds, .05f);    


        camera.Move(offset * kMovementSpeed * deltaSeconds);

        float mouseDeltaX = .0f;
        float mouseDeltaY = .0f;
        SDL_MouseButtonFlags mouseBtns = SDL_GetRelativeMouseState(&mouseDeltaX, &mouseDeltaY);
        if (mouseBtns & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) {
            Vector rotationAxisY = Vector(0, 1, 0) * -mouseDeltaX;
            Vector rotationAxisX = Vector(1, 0, 0) * -mouseDeltaY;

            if (mouseDeltaX or mouseDeltaY) {
                needsRender = true;
                camera.Rotate((rotationAxisY + rotationAxisX).Unit(), kRotationSpeed * deltaSeconds);
            }
            
        }
        if (needsRender) {
            const auto start = std::chrono::steady_clock::now();
            Image img = raytracer.RenderMT(camera, scene);
            const auto end = std::chrono::steady_clock::now();
            const std::chrono::duration<float> elapsed = end - start;
            std::cout << "\rRender time: " << elapsed.count()  << " seconds" << std::flush;

            UploadPixelArrayToTexture(texture, img, width, height);
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
