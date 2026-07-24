#include "Image.hpp"

Image::Image(size_t width, size_t height) 
    : _width{width},
      _height{height},
      _pixels(
          width * height,
          Pixel{0, 0, 0}
      )
{}

Pixel& Image::At(size_t x, size_t y) {
    return _pixels[y * _width + x];
}

int Image::Width() const {
    return _width;
}
int Image::Height() const {
    return _height;
}

const std::vector<Pixel>& Image::Pixels() const {
    return _pixels;
}
