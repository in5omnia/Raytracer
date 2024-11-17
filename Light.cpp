#include "Light.h"

Light::Light() {}

Light::~Light() {}

Vector3 Light::getPosition() const {
	return position;
}

Color Light::getIntensity() const {
	return intensity;
}

PointLight::PointLight(Vector3 position, Color intensity) {
	this->position = position;
	this->intensity = intensity;
}

PointLight::~PointLight() {}
