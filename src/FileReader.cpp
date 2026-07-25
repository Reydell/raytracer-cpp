#include "FileReader.hpp"
#include "Vector.hpp"
#include "MaterialPresets.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

void FileReader::ReadFile(Scene& scene, const std::string& path) const {
    std::ifstream file(path);
    std::string line;

    std::vector<Vector> vertices;
    std::vector<std::array<Vector, 3>> faces;

    while (std::getline(file, line)) {
        std::istringstream parser(line);
        std::string entryType;

        if (!(parser >> entryType)) {
            continue;
        }

        if (entryType == "v") {
            float x, y, z;
            parser >> x >> y >> z;
            vertices.push_back(Vector{x, y, z});
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
                if (indices[0] < 0) {
                    faces.push_back({
                        vertices[runningVertexCount + indices[0]],
                        vertices[runningVertexCount + indices[i + 1]],
                        vertices[runningVertexCount + indices[i + 2]]
                    });
                } else {
                    faces.push_back({
                        vertices[indices[0] - 1],
                        vertices[indices[i + 1] - 1],
                        vertices[indices[i + 2] - 1]
                    });
                }
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
                DEFAULT
            ));
        } else if (entryType == "sphere") {
            float x, y, z, radius;
            parser >> x >> y >> z >> radius;
            std::cout << "Added sphere" << std::endl;
            scene.AddShape(std::make_unique<Sphere>(
                Vector(x, y, z),
                radius,
                GLASS
            ));
        }
    }

    for (const auto& triangle : faces) {
        scene.AddShape(std::make_unique<Triangle>(
            triangle[0],
            triangle[1],
            triangle[2],
            DEFAULT
        ));
    }

    std::cout << "DONE LOADING" << std::endl;
}
