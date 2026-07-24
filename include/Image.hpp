#pragma once

#include <cstddef>
#include <cstdint>
#include <sys/types.h>
#include <vector>

struct Pixel {
public:
    Pixel(std::uint8_t red, std::uint8_t green, std::uint8_t blue)
        : _red(red), 
          _green(green), 
          _blue(blue), 
          _alpha(255) 
    {}

    std::uint8_t Red() const { return _red; }
    std::uint8_t Green() const { return _green; }
    std::uint8_t Blue() const { return _blue; }
    std::uint8_t Alpha() const { return _alpha; }

private:
    std::uint8_t _red;
    std::uint8_t _green;
    std::uint8_t _blue;
    std::uint8_t _alpha;
};

class Image {
private:
    size_t _width;
    size_t _height;
    std::vector<Pixel> _pixels;

public:
    Image(size_t width, size_t height);

    Pixel& At(size_t x, size_t y);

    int Width() const;
    int Height() const;
    const std::vector<Pixel>& Pixels() const;
};
