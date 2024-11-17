#ifndef RAYTRACER_MATERIAL_H
#define RAYTRACER_MATERIAL_H
#include "Vector3.h"
#include "Color.h"

class Material {
	private:
		float ks;                   // Specular coefficient
		float kd;                   // Diffuse coefficient
		int specularExponent;       // Specular exponent
		Color diffuseColor;       // Diffuse color (RGB)
		Color specularColor;      // Specular color (RGB)
		bool isReflective;          // Reflective property
		float reflectivity;         // Reflectivity coefficient
		bool isRefractive;          // Refractive property
		float refractiveIndex;      // Refractive index

	public:
		// Constructor
		Material(float ks, float kd, int specularExponent,
				 const Color& diffuseColor, const Color& specularColor,
				 bool isReflective, float reflectivity,
				 bool isRefractive, float refractiveIndex);

		// Default Constructor
		Material();
		~Material() = default;
		bool getIsReflective() const;
		bool getIsRefractive() const;
		float getKs() const;
		float getKd() const;
		int getSpecularExponent() const;
		Color getDiffuseColor() const;
		Color getSpecularColor() const;
		float getReflectivity() const;
		float getRefractiveIndex() const;
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
