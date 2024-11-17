#include "Shape.h"

#include <iostream>


// Sphere class

Sphere::Sphere(Vector3 center, float radius, Material material) : center(center), radius(radius) {
	this->material = material;
}

Sphere::~Sphere() {}

bool Sphere::intersect(const Ray& ray, float& t) {

	Vector3 L = ray.getOrigin() - center;
	float a = dotProduct(ray.getDirection(), ray.getDirection());  // Should always be 1 if normalized
	float b = 2.0f * dotProduct(L, ray.getDirection());
	float c = dotProduct(L, L) - radius * radius;

	float discriminant = b * b - 4 * a * c;	// Discriminant of the quadratic equation: b^2 - 4ac

	// No intersection
	if (discriminant < 0) {
		//std::cout << "No Intersection :(" << std::endl;
		return false;
	}

	// Intersection
	float sqrtDiscriminant = sqrt(discriminant);
	float t1 = (-b - sqrtDiscriminant) / (2.0f * a);
	float t2 = (-b + sqrtDiscriminant) / (2.0f * a);

	// We want the nearest positive t (after the ray's origin - camera in pinhole model)
	if (t1 > 0) {
		t = t1;
	} else if (t2 > 0) {
		t = t2;
	} else {
		return false;  // Both intersections are behind the ray origin
	}
	std::cout << "Intersection detected in sphere!!" << std::endl;
	return true;
}

Vector3 Sphere::getNormal(const Vector3& point) {
	return (point - center).normalize();
}
