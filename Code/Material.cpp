//
// Created by Beatriz Gavilan on 16/11/2024.
//

#include "Material.h"

Material::Material(float ks, float kd, int specularExponent,
				   const Color& diffuseColor, const Color& specularColor,
				   bool isReflective, float reflectivity,
				   bool isRefractive, float refractiveIndex)
		: ks(ks), kd(kd), specularExponent(specularExponent),
		  diffuseColor(diffuseColor), specularColor(specularColor),
		  isReflective(isReflective), reflectivity(reflectivity),
		  isRefractive(isRefractive), refractiveIndex(refractiveIndex),
		  hasTexture(false) {
}

Material::Material(const Color& diffuseColor, const Color& specularColor,
						bool isReflective, float reflectivity,
						bool isRefractive, float refractiveIndex,
						float roughness, const Color& emission)
		: ks(0.0f), kd(0.0f), specularExponent(0.0f),
		  diffuseColor(diffuseColor), specularColor(specularColor),
		  isReflective(isReflective), reflectivity(reflectivity),
		  isRefractive(isRefractive), refractiveIndex(refractiveIndex),
		  hasTexture(false), roughness((roughness > 0.0f && roughness < 1.0f) ? roughness : 1.0f), emission(emission) {}

// Default Constructor
Material::Material()
		: ks(0.0f), kd(0.0f), specularExponent(1),
		  diffuseColor(Color(0, 0, 0)), specularColor(Color(0, 0, 0)),
		  isReflective(false), reflectivity(0.0f),
		  isRefractive(false), refractiveIndex(1.0f),
		  hasTexture(false){}


bool Material::getIsReflective() const{ return isReflective; }
bool Material::getIsRefractive() const{ return isRefractive; }
float Material::getReflectivity() const{ return reflectivity; }
float Material::getRefractiveIndex() const{ return refractiveIndex; }
float Material::getKs() const{ return ks; }
float Material::getKd() const{ return kd; }
int Material::getSpecularExponent() const{ return specularExponent; }
Color Material::getDiffuseColor() const{ return diffuseColor; }
Color Material::getSpecularColor() const{ return specularColor; }

// Texture-related methods
bool Material::hasTextureMap() const { return hasTexture; }
void Material::setTexture(Image& tex) {
	texture = tex;
	hasTexture = (tex.getWidth() != 0 && tex.getHeight() != 0);
}
const Image& Material::getTexture() const { return texture; }

// BRDF methods
float Material::getRoughness() const {
	return roughness;
}

void Material::setRoughness(float roughness) {
	this->roughness = roughness;
}

Color Material::getEmission() const {
	return emission;
}

bool Material::isGlossy() const {
	//return roughness > 0.0f && roughness < 1.0f; // Glossy materials have roughness > 0
	return !isReflective && !isRefractive && roughness < 1.0f && roughness > 0.0f;
}

bool Material::isDiffuse() const {
	//return roughness == 1.0f || (!isReflective && !isGlossy()); // Fully diffuse if no glossiness or reflection
	return !isReflective && !isRefractive && roughness >= 1.0f;
}

bool Material::hasDiffuse() const {
	return diffuseColor.getR() > 0.0f || diffuseColor.getG() > 0.0f || diffuseColor.getB() > 0.0f;
}

