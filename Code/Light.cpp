#include "Light.h"

PointLight::PointLight() {}

PointLight::PointLight(Vector3 position, Color intensity)
			: position(position), intensity(intensity) {}

PointLight::~PointLight() {}

Vector3 PointLight::getPosition() const {
	return position;
}

Color PointLight::getIntensity() const {
	return intensity;
}

void PointLight::sampleLight(const Vector3& referencePoint,
							 std::default_random_engine& rng,
							 Vector3& lightPosition,
							 Vector3& lightNormal,
							 float& pdf) const {
	// For point lights
	lightPosition = position;
	lightNormal = - (lightPosition - referencePoint).normalize();
	pdf = 1.0f;  // Since it's a single point
}

Color PointLight::getIntensity(const Vector3& intersectionPoint,
							   const Vector3& lightPosition,
							   const Vector3& lightNormal) const {
	return intensity;
}

