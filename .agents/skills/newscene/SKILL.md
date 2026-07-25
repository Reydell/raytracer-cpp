---
name: newscene
description: Create a new raytracer OBJ/MTL test scene with fully initialized materials from an explicit `$newscene "Scene Name" scene description` invocation. Use when the user wants a described scene generated as `tests/{scene name}/scene.obj` and `tests/{scene name}/scene.mtl` in this repository.
---

# New Scene

Create one self-contained test scene from two logical arguments:

```text
$newscene "<scene name>" <prompt describing the scene>
```

## Parse the invocation

1. Treat the first quoted string after `$newscene` as the scene name.
2. Treat the entire non-empty remainder as the scene prompt. Do not split it into
   separate arguments; remove one matching pair of surrounding quotes if present.
3. If either value is missing or the name is not quoted, ask for a corrected
   invocation and stop.
4. Preserve the scene name's spelling, capitalization, and spaces. Reject names
   containing `/`, `\`, control characters, or the special values `.` and `..`.

## Use the fixed output layout

Resolve the repository root and target this exact directory:

```text
<repo root>/tests/<scene name>/
```

Create exactly these scene files inside it:

```text
scene.obj
scene.mtl
```

Do not modify `src/main.cpp`, other source files, or existing test scenes unless
the user separately requests it. If the target folder or either file already
exists, inspect it and obtain confirmation before overwriting or mixing content.

## Match the repository's scene dialect

Before authoring, inspect `src/FileReader.cpp`, `include/Material.hpp`,
`src/Material.cpp`, `src/RayTracer.cpp`, the active camera setup, and a small
selection of existing scenes under `tests/`. Follow the current parser rather
than assuming full Wavefront support.

At the time this skill was authored, the scene loader recognizes:

- OBJ: `mtllib`, `v`, `usemtl`, `f`
- Repo scene extensions: `S x y z radius`, `plane x y z nx ny nz`,
  and `P x y z r g b`
- MTL: `newmtl`, `Ns`, `Ni`, `d`, `Tr`, `Tf`, `Ka`, `Kd`, `Ks`, and `Ke`
- Repo MTL extension: `al lightsConstant reflectivity transparency`

Re-check these capabilities on every invocation because the implementation may
change. Do not rely on ignored directives.

## Initialize every material field

Write every `newmtl` block with exactly one of each directive below, even when a
value is zero or irrelevant to that material:

```text
newmtl MaterialName
Ke 0 0 0
Ka 0 0 0
Kd 0 0 0
Ks 0 0 0
Tf 1 1 1
Ni 1.0
al 1.0 0.0 0.0
Ns 300
```

The loader maps those entries to all `Material` fields:

- `Ke` -> `emitted`
- `Ka` -> `ambient`
- `Kd` -> `diffused`
- `Ks` -> `specular`
- `Tf` -> transmitted `tint`
- `Ni` -> `refractionIndex`
- `al` values -> `lightsConstant`, `reflectivity`, and `transparency`, in order
- `Ns` -> `specExponent`

Do not use `d` or `Tr` in generated materials. Both write `transparency`, which
is already initialized by the third `al` value, so combining them makes the
result depend on directive order. Keep color and `al` components within
`0..1`, require `Ni > 0` and `Ns >= 0`, and require
`reflectivity + transparency <= 1`. The loader assigns `al` directly after
constructing the material, so it does not apply the constructor's reflectivity
clamp.

## Author the scene

1. Put `mtllib scene.mtl` near the start of `scene.obj`.
2. Add a short comment naming the scene and, when useful, the intended camera
   position and forward vector.
3. Translate the prompt into visible geometry, materials, and lighting using only
   parser-supported directives.
4. Apply `usemtl` before every group of shapes that needs a material.
5. Define every referenced material in `scene.mtl` and initialize every field
   using the complete material block above. Never depend on `Material{}`
   defaults.
6. Include enough lighting or emissive material for the requested scene to read
   clearly. Keep geometry within the intended camera's view.
7. Use comments sparingly and keep both files easy to edit by hand.

## Validate before finishing

- Confirm the folder name exactly matches the parsed scene name.
- Confirm only `scene.obj` and `scene.mtl` were created in the new folder.
- Confirm `scene.obj` references `scene.mtl`.
- Confirm every `usemtl` name has exactly one corresponding `newmtl`.
- Confirm every `newmtl` block has exactly one `Ke`, `Ka`, `Kd`, `Ks`, `Tf`,
  `Ni`, `al`, and `Ns`, and contains neither `d` nor `Tr`.
- Confirm all material values meet the ranges and
  `reflectivity + transparency <= 1`.
- Check directive arity, numeric values, positive sphere radii, non-zero plane
  normals, face indices, and face winding.
- Run `git diff --check` on both files.
- Report the created scene and provide links to both files.
