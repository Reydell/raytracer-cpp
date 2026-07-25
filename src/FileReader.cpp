#include "FileReader.hpp"
#include "Material.hpp"
#include "Vector.hpp"
#include "MaterialPresets.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

std::unordered_map<std::string, Material> FileReader::ReadMtl(const std::string& path) const {
    std::unordered_map<std::string, Material> mtlMap;
    std::ifstream file(path);
    std::string line;
    bool flag = false;
    std::string newName;
    Material newMaterial; // DON'T FORGET TO UPDATE PARAMETERS LIKE IN CONSTRUCTOR
    while (std::getline(file, line)) {
        std::istringstream parser(line);
        std::string entryType;

        if (!(parser >> entryType)) {
            continue;
        }

        if (entryType == "newmtl") {
            if (flag) {
                mtlMap.emplace(newName, newMaterial);
            }
            parser >> newName;
            newMaterial = Material{};
            flag = true;
        } else if (entryType.starts_with('#')) {
            continue;
        } else {
            if (entryType == "Ns") {
                float x;
                parser >> x;
                newMaterial.specExponent = x;
            } else if (entryType == "Ni") {
                float x;
                parser >> x;
                newMaterial.refractionIndex = x;
            } else if (entryType == "d") {
                float x;
                parser >> x;
                newMaterial.transparency = 1 - x;
            } else if (entryType == "Tr") {
                float x;
                parser >> x;
                newMaterial.transparency = x;
            } else if (entryType == "Tf") {
                float r, g, b;
                parser >> r >> g >> b;
                newMaterial.tint = Color{r, g, b};
            } else if (entryType == "Ka") {
                float r, g, b;
                parser >> r >> g >> b;
                newMaterial.ambient = Color{r, g, b};
            } else if (entryType == "Kd") {
                float r, g, b;
                parser >> r >> g >> b;
                newMaterial.diffused = Color{r, g, b};
            } else if (entryType == "Ks") {
                float r, g, b;
                parser >> r >> g >> b;
                newMaterial.specular = Color{r, g, b};
            } else if (entryType == "Ke") {
                float r, g, b;
                parser >> r >> g >> b;
                newMaterial.emitted = Color{r, g, b};
            }
        }

    }

    mtlMap.emplace(newName, newMaterial);

    return mtlMap;

}

void FileReader::ReadFile(Scene& scene, const std::string& objPath) const {
    std::ifstream file(objPath);
    std::string line;
    const std::filesystem::path objDirectory =
        std::filesystem::path(objPath).parent_path();

    std::vector<Vector> vertices;
    std::vector<std::array<Vector, 3>> faces;

    std::unordered_map<std::string, Material> mtlMap;
    Material currentMtl;

    while (std::getline(file, line)) {
        std::istringstream parser(line);
        std::string entryType;

        if (!(parser >> entryType)) {
            continue;
        }
        if (entryType == "mtllib") {
            std::string mtlPath;
            parser >> mtlPath;
            mtlMap = ReadMtl((objDirectory / mtlPath).string());
        } else if (entryType == "v") {
            float x, y, z;
            parser >> x >> y >> z;
            vertices.push_back(Vector{x, y, z});
        } else if (entryType == "usemtl") {
            std::string mtlName;
            parser >> mtlName;
            currentMtl = mtlMap[mtlName];
        } else if (entryType == "f") {
            std::vector<int32_t> indices;
            std::string token;

            while (parser >> token) {
                if (token.starts_with('#')) {
                    break;
                }

                const std::size_t slash = token.find('/');
                const std::string actualIndex = token.substr(0, slash);

                indices.push_back(std::stoi(actualIndex));
            }

            size_t nIterations = indices.size() - 2;
            size_t runningVertexCount = vertices.size();
            for (size_t i = 0; i < nIterations; ++i) {
                // if (indices[0] < 0) {
                //     faces.push_back({
                //         vertices[runningVertexCount + indices[0]],
                //         vertices[runningVertexCount + indices[i + 1]],
                //         vertices[runningVertexCount + indices[i + 2]]
                //     });
                // } else {
                //     faces.push_back({
                //         vertices[indices[0] - 1],
                //         vertices[indices[i + 1] - 1],
                //         vertices[indices[i + 2] - 1]
                //     });
                // }
                std::array<Vector, 3> newFace;
                if (indices[0] < 0) {
                    newFace[0] = vertices[runningVertexCount + indices[0]];
                    newFace[1] = vertices[runningVertexCount + indices[i + 1]];
                    newFace[2] = vertices[runningVertexCount + indices[i + 2]];
                } else {
                    newFace[0] = vertices[indices[0] - 1];
                    newFace[1] = vertices[indices[i + 1] - 1];
                    newFace[2] = vertices[indices[i + 2] - 1];
                }

                scene.AddShape(std::make_unique<Triangle>(
                    newFace[0],
                    newFace[1],
                    newFace[2],
                    currentMtl
                ));
            }

        } else if (entryType == "P") {
            float x, y, z, r, g, b;
            parser >> x >> y >> z >> r >> g >> b;

            scene.AddLight(std::make_unique<LightSource>(
                Vector{x, y, z},
                Color{r, g, b}
            ));
        } else if (entryType == "plane") {
            float x, y, z, nx, ny, nz;
            parser >> x >> y >> z >> nx >> ny >> nz;

            scene.AddShape(std::make_unique<Plane>(
                Vector(x, y, z),
                Vector(nx, ny, nz),
                currentMtl
            ));
        } else if (entryType == "S") {
            float x, y, z, radius;
            parser >> x >> y >> z >> radius;
            std::cout << "Added sphere" << std::endl;
            scene.AddShape(std::make_unique<Sphere>(
                Vector(x, y, z),
                radius,
                currentMtl
            ));
        }
    }

    // for (const auto& triangle : faces) {
    //     scene.AddShape(std::make_unique<Triangle>(
    //         triangle[0],
    //         triangle[1],
    //         triangle[2],
    //         DEFAULT
    //     ));
    // }

    std::cout << "DONE LOADING" << std::endl;
}
