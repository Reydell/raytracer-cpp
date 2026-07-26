#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

namespace {

// The pixel array and the SDL texture always have this fixed, logical size.
// SDL scales the texture if the user resizes the actual window.
constexpr int kCanvasWidth = 800;
constexpr int kCanvasHeight = 600;

constexpr int kCircleRadius = 30;
constexpr float kCircleSpeed = 250.0F; // Logical pixels per second.

// SDL_PIXELFORMAT_RGBA32 expects these four bytes in exactly this order.
struct Pixel {
  std::uint8_t red;
  std::uint8_t green;
  std::uint8_t blue;
  std::uint8_t alpha;
};

static_assert(sizeof(Pixel) == 4);

using PixelArray = std::vector<Pixel>;

constexpr Pixel kBackgroundColor{18, 20, 30, 255};
constexpr Pixel kCircleColor{255, 170, 40, 255};

// Create width * height pixels stored in one contiguous array.
// Pixel (x, y) is located at index y * width + x.
PixelArray CreatePixelArray(int width, int height, Pixel initialColor) {
  const auto pixelCount =
      static_cast<std::size_t>(width) * static_cast<std::size_t>(height);

  return PixelArray(pixelCount, initialColor);
}

void ClearPixelArray(PixelArray &pixels, Pixel color) {
  std::fill(pixels.begin(), pixels.end(), color);
}

// Change exactly one pixel. The bounds check prevents writing outside the
// vector if a shape extends past an edge of the canvas.
void PutPixel(PixelArray &pixels, int width, int height, int x, int y,
              Pixel color) {
  if (x < 0 || x >= width || y < 0 || y >= height) {
    return;
  }

  const auto index =
      static_cast<std::size_t>(y) * static_cast<std::size_t>(width) +
      static_cast<std::size_t>(x);

  pixels[index] = color;
}

// Rasterize a filled circle into the CPU pixel array.
//
// A pixel is inside a circle when:
//     (x - centerX)^2 + (y - centerY)^2 <= radius^2
//
// This function does not ask SDL to draw a circle. It visits the pixels in
// the circle's bounding square and writes every covered pixel itself.
void DrawFilledCircle(PixelArray &pixels, int width, int height, int centerX,
                      int centerY, int radius, Pixel color) {
  const int radiusSquared = radius * radius;

  const int firstX = std::max(0, centerX - radius);
  const int lastX = std::min(width - 1, centerX + radius);
  const int firstY = std::max(0, centerY - radius);
  const int lastY = std::min(height - 1, centerY + radius);

  for (int y = firstY; y <= lastY; ++y) {
    for (int x = firstX; x <= lastX; ++x) {
      const int offsetX = x - centerX;
      const int offsetY = y - centerY;
      const int distanceSquared = offsetX * offsetX + offsetY * offsetY;

      if (distanceSquared <= radiusSquared) {
        PutPixel(pixels, width, height, x, y, color);
      }
    }
  }
}

// Copy the CPU pixel array into an SDL streaming texture.
//
// SDL supplies "pitch": the number of bytes between the beginnings of two
// neighboring texture rows. Pitch can be larger than width * sizeof(Pixel),
// so each row must be copied separately.
bool UploadPixelArrayToTexture(SDL_Texture *texture, const PixelArray &pixels,
                               int width, int height) {
  void *textureMemory = nullptr;
  int texturePitch = 0;

  if (!SDL_LockTexture(texture, nullptr, &textureMemory, &texturePitch)) {
    std::cerr << "SDL_LockTexture failed: " << SDL_GetError() << '\n';
    return false;
  }

  auto *destinationBytes = static_cast<std::uint8_t *>(textureMemory);
  const std::size_t bytesPerRow =
      static_cast<std::size_t>(width) * sizeof(Pixel);

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

} // namespace

int main(int argc, char *argv[]) {
  // These are supplied by the operating system but unused in this example.
  static_cast<void>(argc);
  static_cast<void>(argv);

  // Initialize SDL's video subsystem. Almost every SDL function reports
  // details about its most recent failure through SDL_GetError().
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow(
      "SDL3 CPU pixels - move with WASD", kCanvasWidth, kCanvasHeight,
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);

  if (window == nullptr) {
    std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
    SDL_Quit();
    return 1;
  }

  // nullptr means: let SDL choose the best available 2D renderer.
  SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);

  if (renderer == nullptr) {
    std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << '\n';
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Keep the game coordinate system at 800x600 even when the physical
  // window is resized. SDL adds letterboxing when aspect ratios differ.
  if (!SDL_SetRenderLogicalPresentation(renderer, kCanvasWidth, kCanvasHeight,
                                        SDL_LOGICAL_PRESENTATION_LETTERBOX)) {
    std::cerr << "SDL_SetRenderLogicalPresentation failed: " << SDL_GetError()
              << '\n';
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // VSync prevents this small program from rendering thousands of frames
  // per second. Some renderers cannot provide it, so failure is non-fatal.
  if (!SDL_SetRenderVSync(renderer, 1)) {
    std::cerr << "Warning: VSync is unavailable: " << SDL_GetError() << '\n';
  }

  // STREAMING means that the CPU will replace the texture pixels often.
  SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           kCanvasWidth, kCanvasHeight);

  if (texture == nullptr) {
    std::cerr << "SDL_CreateTexture failed: " << SDL_GetError() << '\n';
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Preserve visibly sharp pixels when SDL scales the 800x600 texture.
  if (!SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST)) {
    std::cerr << "SDL_SetTextureScaleMode failed: " << SDL_GetError() << '\n';
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  PixelArray pixels =
      CreatePixelArray(kCanvasWidth, kCanvasHeight, kBackgroundColor);

  float circleX = static_cast<float>(kCanvasWidth) / 2.0F;
  float circleY = static_cast<float>(kCanvasHeight) / 2.0F;

  bool running = true;
  int exitCode = 0;
  Uint64 previousTicks = SDL_GetTicks();

  std::cout << "Move with W, A, S, D. Quit with Escape or close the window.\n";

  while (running) {
    // Poll every queued operating-system event once per frame.
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running = false;
      }

      if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
        running = false;
      }
    }

    // SDL_GetKeyboardState is convenient for continuous movement. Unlike
    // a KEY_DOWN event, it remains true for as long as a key is held.
    const bool *keyboard = SDL_GetKeyboardState(nullptr);

    float movementX = 0.0F;
    float movementY = 0.0F;

    if (keyboard[SDL_SCANCODE_A]) {
      movementX -= 1.0F;
    }
    if (keyboard[SDL_SCANCODE_D]) {
      movementX += 1.0F;
    }
    if (keyboard[SDL_SCANCODE_W]) {
      movementY -= 1.0F;
    }
    if (keyboard[SDL_SCANCODE_S]) {
      movementY += 1.0F;
    }

    const Uint64 currentTicks = SDL_GetTicks();
    float deltaSeconds =
        static_cast<float>(currentTicks - previousTicks) / 1000.0F;
    previousTicks = currentTicks;

    // Avoid a large movement jump if execution pauses in the debugger.
    deltaSeconds = std::min(deltaSeconds, 0.05F);

    // Normalize diagonal movement so W+D is not faster than W alone.
    const float movementLengthSquared =
        movementX * movementX + movementY * movementY;

    if (movementLengthSquared > 0.0F) {
      const float inverseLength = 1.0F / std::sqrt(movementLengthSquared);
      movementX *= inverseLength;
      movementY *= inverseLength;

      circleX += movementX * kCircleSpeed * deltaSeconds;
      circleY += movementY * kCircleSpeed * deltaSeconds;
    }

    // Keep the complete circle inside the logical canvas.
    circleX = std::clamp(circleX, static_cast<float>(kCircleRadius),
                         static_cast<float>(kCanvasWidth - kCircleRadius - 1));
    circleY = std::clamp(circleY, static_cast<float>(kCircleRadius),
                         static_cast<float>(kCanvasHeight - kCircleRadius - 1));

    // Rebuild the complete CPU image every frame.
    ClearPixelArray(pixels, kBackgroundColor);
    DrawFilledCircle(pixels, kCanvasWidth, kCanvasHeight,
                     static_cast<int>(std::lround(circleX)),
                     static_cast<int>(std::lround(circleY)), kCircleRadius,
                     kCircleColor);

    if (!UploadPixelArrayToTexture(texture, pixels, kCanvasWidth,
                                   kCanvasHeight)) {
      exitCode = 1;
      break;
    }

    // SDL rendering goes to a backbuffer. Clear it, copy the complete
    // pixel texture to it, then present the finished frame to the window.
    if (!SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) ||
        !SDL_RenderClear(renderer) ||
        !SDL_RenderTexture(renderer, texture, nullptr, nullptr) ||
        !SDL_RenderPresent(renderer)) {
      std::cerr << "SDL rendering failed: " << SDL_GetError() << '\n';
      exitCode = 1;
      break;
    }
  }

  // Destroy resources in reverse order of creation.
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return exitCode;
}
