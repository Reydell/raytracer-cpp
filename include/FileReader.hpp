#pragma once

#include "Scene.hpp"
#include "Shape.hpp"

#include <string>

class FileReader {
public:
    void ReadFile(Scene& scene, const std::string& path) const;

    // later may add more complicated readers
};