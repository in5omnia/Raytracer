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
		  isRefractive(isRefractive), refractiveIndex(refractiveIndex) {}

// Default Constructor
Material::Material()
		: ks(0.0f), kd(0.0f), specularExponent(1),
		  diffuseColor(Color(0, 0, 0)), specularColor(Color(0, 0, 0)),
		  isReflective(false), reflectivity(0.0f),
		  isRefractive(false), refractiveIndex(1.0f) {}


bool Material::getIsReflective() const{ return isReflective; }
bool Material::getIsRefractive() const{ return isRefractive; }
float Material::getReflectivity() const{ return reflectivity; }
float Material::getRefractiveIndex() const{ return refractiveIndex; }
float Material::getKs() const{ return ks; }
float Material::getKd() const{ return kd; }
int Material::getSpecularExponent() const{ return specularExponent; }
Color Material::getDiffuseColor() const{ return diffuseColor; }
Color Material::getSpecularColor() const{ return specularColor; }
