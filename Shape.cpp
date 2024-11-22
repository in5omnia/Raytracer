#include "Shape.h"

#include <iostream>

/* Shape class */
Shape::Shape(const Material& material): material(material) {}

Material Shape::getMaterial() const { return material; }

/* Sphere class */

Sphere::Sphere(Vector3 center, float radius, const Material& material) : Shape(material), center(center), radius(radius) {}


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
	//std::cout << "Intersection detected in sphere!!" << std::endl;
	return true;
}

Vector3 Sphere::getNormal(const Vector3& point) {
	return (point - center).normalize();
}

/* Cylinder class */

Cylinder::Cylinder(Vector3 center, Vector3 axis, float radius, float height, const Material& material) :
				Shape(material), center(center), axis(axis.normalize()), radius(radius), height(height*2.0) {}	//multiply height to match cw image


bool Cylinder::intersect(const Ray& ray, float& t) {
	Vector3 V = ray.getOrigin() - center;  // Vector from cylinder center to ray origin

	// Intersect with the curved surface
	Vector3 dPerp = ray.getDirection() - axis * dotProduct(ray.getDirection(), axis);
	Vector3 vPerp = V - axis * dotProduct(V, axis);

	float a = dotProduct(dPerp, dPerp);
	float b = 2.0f * dotProduct(dPerp, vPerp);
	float c = dotProduct(vPerp, vPerp) - radius * radius;

	float discriminant = b * b - 4 * a * c;
	if (discriminant < 0) return false;  // No intersection
	float tCurved = INFINITY;

	if (discriminant >= 0) {  // Curved surface intersection
		float sqrtDisc = sqrt(discriminant);
		float t1 = (-b - sqrtDisc) / (2.0f * a);
		float t2 = (-b + sqrtDisc) / (2.0f * a);

		// Validate intersection points for the curved surface
		Vector3 p1 = ray.pointAtParameter(t1);
		Vector3 p2 = ray.pointAtParameter(t2);

		if (t1 > 0 && isWithinHeight(p1)) {
			tCurved = t1;
		}
		if (t2 > 0 && isWithinHeight(p2) && t2 < tCurved) {
			tCurved = t2;
		}
	}

	// Intersect with the top base
	Vector3 topCenter = center + axis * (height / 2.0f);	//center of the top base
	float denominator = dotProduct(ray.getDirection(), axis);	//component of the ray's direction vector along the axis of the cylinder
	float tTop = INFINITY;

	//Check if the ray is parallel to the plane of the top base (when denomTop = 0) to avoid division by zero
	if (fabs(denominator) > 1e-6) {
		tTop = dotProduct(topCenter - ray.getOrigin(), axis) / denominator;
		Vector3 pTop = ray.pointAtParameter(tTop);
		if ((pTop - topCenter).norm() > radius || tTop <= 0) {  // Point outside disk or behind ray
			tTop = INFINITY;
		}
	}

	// Intersect with the bottom base
	Vector3 bottomCenter = center - axis * (height / 2.0f);
	float tBottom = INFINITY;
	if (fabs(denominator) > 1e-6) {  // Avoid division by zero
		tBottom = dotProduct(bottomCenter - ray.getOrigin(), axis) / denominator;
		Vector3 pBottom = ray.pointAtParameter(tBottom);
		if ((pBottom - bottomCenter).norm() > radius || tBottom <= 0) {  // Point outside disk or behind ray
			tBottom = INFINITY;
		}
	}

	// Determine the nearest valid intersection
	t = std::min({tCurved, tTop, tBottom});
	return t < INFINITY;
}


bool Cylinder::isWithinHeight(const Vector3& point) const {
	// Project point onto the cylinder axis relative to the center
	float projection = dotProduct((point - center), axis);

	// Check if the projection lies within the height bounds
	return projection >= -height / 2 && projection <= height / 2;
}

Vector3 Cylinder::getNormal(const Vector3& point){
	// Check if the point is on the top or bottom base
	float projectionHeight = dotProduct(point - center, axis);
	if (projectionHeight >= height / 2.0f) {
		// Point is on the top base
		return axis;
	} else if (projectionHeight <= -height / 2.0f) {
		// Point is on the bottom base
		return (axis * (-1.0f));
	}

	// Project the point onto the cylinder's axis
	Vector3 projectionOnAxis = center + axis * projectionHeight;

	// Compute the normal vector
	Vector3 normal = point - projectionOnAxis;

	// Normalize the normal vector
	return normal.normalize();
}

/* Triangle */

Triangle::Triangle(Vector3 v0, Vector3 v1, Vector3 v2, const Material& material) :
										Shape(material), v0(v0), v1(v1), v2(v2) {}


Vector3 Triangle::getNormal(const Vector3& rayDir) {	//Note that here point is the direction of the ray
	Vector3 E1 = v1 - v0;  // Edge 1: from v0 to v1
	Vector3 E2 = v2 - v0;  // Edge 2: from v0 to v2
	Vector3 normal = crossProduct(E1, E2).normalize();
	if (dotProduct(normal, rayDir) > 0) {	//ensure normal is pointing towards the ray origin (camera)
		return normal * -1.0f;
	}
	return normal;
}

bool Triangle::intersect(const Ray& ray, float& t) {
	Vector3 rayDir = ray.getDirection();
	Vector3 E1 = v1 - v0;  // Edge 1: from v0 to v1
	Vector3 E2 = v2 - v0;  // Edge 2: from v0 to v2
	Vector3 P = crossProduct(rayDir, E2);  // P = D x E2

	// calculate determinant
	float determinant = dotProduct(E1, P);  // det[-D, E1, E2] = E1 . P

	// Check if Ray is parallel to triangle
	if (fabs(determinant) < 1e-8) return false;

	float invDet = 1.0f / determinant;  // (1/det)
	Vector3 T = ray.getOrigin() - v0;  // Vector from v0 to ray origin

	// Calculate u barycentric coordinate
	float u = invDet * dotProduct(T, P);
	if (u < 0 || u > 1) return false;  // Check if u is outside [0, 1]

	Vector3 Q = crossProduct(T, E1);  // Q = T x E1

	// Calculate v barycentric coordinate
	float v = invDet * dotProduct(rayDir, Q);
	if (v < 0 || u + v > 1) return false;  // Check if v is outside valid range

	// Calculate t (intersection distance along ray)
	t = invDet * dotProduct(E2, Q);
	return t > 0;  // Intersection is valid if t is positive
}
