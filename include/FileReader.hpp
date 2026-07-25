#pragma once

#include "Scene.hpp"
#include "Shape.hpp"
#include "Camera.hpp"

#include <string>
#include <unordered_map>

class FileReader {
public:
    std::unordered_map<std::string, Material> ReadMtl(const std::string& path) const;
    void ReadFile(Scene& scene, const std::string& objPath) const;

    // later may add more complicated readers
};