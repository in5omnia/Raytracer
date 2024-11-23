#include "Light.h"

PointLight::PointLight() {}

PointLight::PointLight(Vector3 position, Color intensity) : position(position), intensity(intensity) {}

PointLight::~PointLight() {}

Vector3 PointLight::getPosition() const {
	return position;
}

Color PointLight::getIntensity() const {
	return intensity;
}


