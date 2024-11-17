#ifndef RAYTRACER_SHAPE_H
#define RAYTRACER_SHAPE_H
#include "Material.h"
#include "Ray.h"
#include "Vector3.h"


class Shape {
	protected:
		Material material;
	public:
		virtual ~Shape() = default;
		virtual bool intersect(const Ray& ray, float& t) = 0;
		//Pure virtual function for intersection test.
		virtual Vector3 getNormal(const Vector3& point) = 0;
		//Returns the surface normal at a point.
};


class Sphere : public Shape {
private:
	Vector3 center;
	float radius;
public:
	Sphere(Vector3 center, float radius, Material material);
	~Sphere(); // destructor

	//methods
	bool intersect(const Ray& ray, float& t) override;
	Vector3 getNormal(const Vector3& point) override;
};



#endif //RAYTRACER_SHAPE_H



/*Derived Shape Classes
Sphere

Attributes:
Vector3 center;
float radius;
Methods:
Implements intersect and getNormal.
Triangle

Attributes:
Vector3 v0, v1, v2; // Vertices
Methods:
Implements intersect and getNormal.
Cylinder

Attributes:
Vector3 center;
Vector3 axis; // Direction vector
float radius;
float height;
Methods:
Implements intersect and getNormal.
*/