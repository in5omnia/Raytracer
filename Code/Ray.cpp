#include "Ray.h"

// constructor
Ray::Ray(Vector3 origin, Vector3 direction) : origin(origin), direction(direction.normalize()) {}

// destructor
Ray::~Ray(){}

Vector3 Ray::pointAtParameter(float t) const {
	return origin + direction * t;
}

//getters
Vector3 Ray::getOrigin() const { return origin; }
Vector3 Ray::getDirection() const { return direction; }


Vector3 Ray::calculateReflectionDir(const Vector3& normal) const {
	return direction - normal * 2.0f * (dotProduct(direction, normal));
}

Ray Ray::reflect(const Vector3& normal, const Vector3& intersectionPoint) const {
	Vector3 reflectDir =  calculateReflectionDir(normal);
	reflectDir = reflectDir.normalize();
	return Ray(intersectionPoint + normal * 1e-4, reflectDir);
}

