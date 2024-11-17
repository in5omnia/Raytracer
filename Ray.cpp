#include "Ray.h"

// constructor
Ray::Ray(Vector3 origin, Vector3 direction) : origin(origin), direction(direction) {}

// destructor
Ray::~Ray(){}

Vector3 Ray::pointAtParameter(float t) const {
	return origin + direction * t;
}

//getters
Vector3 Ray::getOrigin() const { return origin; }
Vector3 Ray::getDirection() const { return direction; }

