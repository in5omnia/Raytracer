# Raytracer Development

This repository contains the implementation of a **raytracer** developed for the **Computer Graphics coursework** at the University of Edinburgh.  
The project incrementally builds a renderer with both **basic ray tracing features** (shadows, reflection, refraction) and **advanced global illumination** techniques (sampling and path tracing).

---

## Project Overview

Implemented features:

- **Core Raytracing**
    - Ray generation via a pinhole camera
    - Intersection tests for spheres, cylinders, and triangles
    - JSON-based scene description
- **Shading Models**
    - Blinn-Phong shading
    - Shadows
    - Reflections & Refractions
    - Linear tone mapping
- **Textures**
    - PPM image textures (with high render cost)
- **Acceleration**
    - Bounding Volume Hierarchy (BVH) with Axis-Aligned Bounding Boxes
- **Sampling**
    - Pixel sampling (anti-aliasing)
    - Lens sampling (depth of field)
- **Global Illumination**
    - Initial implementation of BRDF sampling and path tracing

---

## Repository Structure

```text
.
├── Raytracer.cpp / .h    # Core raytracer logic: input, tracing, shading, path tracing
├── Scene.cpp / .h        # Scene graph, BVH traversal, intersection handling
├── Shape.cpp / .h        # Sphere, Cylinder, Triangle classes + wrapper Shape
├── BVH.cpp / .h          # Bounding Volume Hierarchy and AABB classes
├── Camera.cpp / .h       # PinholeCamera class, ray generation
├── Light.cpp / .h        # Light source class
├── Material.cpp / .h     # Material class (diffuse, reflective, refractive)
├── Ray.cpp / .h          # Ray class
├── Color.cpp / .h        # Color operations, clamping, tone mapping
├── Image.cpp / .h        # Image class for PPM loading/writing
├── Vector3.h             # Vector operations
├── json.hpp              # External JSON parsing library
└── README.md
```

## Results Summary

- **BVH acceleration** reduced render time from ~6s -> ~1s for test scenes.
- **Texture mapping** works but incurs very long render times (several hours).
- **Sampling** improves realism:
  - Pixel sampling smooths jagged edges and shadows.
  - Lens sampling creates natural depth-of-field blur.
- **BRDF sampling** partially implemented: capable of glossy surfaces, but not a fully correct Monte Carlo integration.

---

## Notes

- This project was developed as **coursework**, not as a production renderer.
- Code is modular and extendible but may require adjustment to run in new environments.
- Rendering performance varies widely depending on features enabled (especially textures and BRDF path tracing).
- Work involved incremental development, debugging, and testing with assistance from Large Language Models for understanding algorithms and generating code templates.
