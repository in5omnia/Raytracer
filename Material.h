#ifndef RAYTRACER_MATERIAL_H
#define RAYTRACER_MATERIAL_H


class Material {
/*
Attributes:
Diffuse color (Vector3 diffuseColor;)
Specular color (Vector3 specularColor;)
Ambient color (optional)
Specular exponent (float specularExponent;)
Reflectivity (float reflectivity;)
Refractive index (float refractiveIndex;)
Flags for reflective and refractive properties.

Methods:
bool isReflective() const;
bool isRefractive() const;
 */
};


#endif //RAYTRACER_MATERIAL_H


/*
Reflections:

If the material is reflective, cast a secondary ray to compute the reflection.
Combine this reflected color with the surface's own color.
Refractions:

For transparent materials, trace a refracted ray.
Use Snell’s Law to calculate the bending of light.
Global Illumination:

Use path tracing to simulate indirect lighting (light bouncing off multiple surfaces).
*/
