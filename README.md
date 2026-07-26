# Raytracer

`en`

## Images
<img width="2560" height="1575" alt="perfectglasstest" src="https://github.com/user-attachments/assets/95207d6b-8ad1-4588-84ff-9c97600b81df" />

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

```sh
make release <folder>
make release <folder> -- -r 3 -w 1024 -h 768
```

- `folder` is the name of the folder inside `tests/` (e.g `ballcube`)
- `-r` is the recursion depth (technically it's the number of steps)
- `-w` is the image width (default: `800`)
- `-h` is the image height (default: `600`)

The `--` is required before the flags because `make` has its own `-h` option.
Dimensions can also be passed as Make variables:

```sh
make debug <folder> WIDTH=1024 HEIGHT=768
```

Currently minimal recursion depth is 1 (which means only the primary ray is calculated)

# TODO

- light occlusion and distance to source
