# Raytracer

`en`

## Features

1. Rendering spheres, triangles and planes
2. Loader for `.obj` and `.mtl` files
3. Balanced multithreaded rendering (currently no GPU support)
4. Bounding volume hierarchy
5. Movement (WASD) and camera rotation (hold LMB + drag)
6. Codex skill `$newscene "scene_name" prompt` to generate new scenes

## Requirements

- CMake 3.20 or newer
- A C++20-compatible compiler
- SDL3 with CMake package files
- POSIX threads
- Make (optional, for using the provided `Makefile`)

The provided `Makefile` expects the compiler at `/usr/bin/clang++`.

## Run

`make release <folder> -r 3`

- `folder` is the name of the folder inside `tests/` (e.g `ballcube`)
- `-r` is the recursion depth (technically it's the number of steps)

Currently minimal recursion depth is 1 (which means only the primary ray is calculated)
