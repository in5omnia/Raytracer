#ifndef RAYTRACER_SHAPE_H
#define RAYTRACER_SHAPE_H
#include "Material.h"
#include "Ray.h"
#include "Vector3.h"


class Shape {
	protected:
		Material material;
	public:
		Shape(const Material& material);
		virtual ~Shape() = default;
		virtual bool intersect(const Ray& ray, float& t) = 0;
		Material getMaterial() const;
		//Pure virtual function for intersection test.
		virtual Vector3 getNormal(const Vector3& point) = 0; //note: triangle doesnt use point
		//Returns the surface normal at a point.
};


class Sphere : public Shape {
	private:
		Vector3 center;
		float radius;
	public:
		Sphere(Vector3 center, float radius, const Material& material);

		//methods
		bool intersect(const Ray& ray, float& t) override;
		Vector3 getNormal(const Vector3& point) override;
};


class Cylinder : public Shape {
	private:
		Vector3 center;
		Vector3 axis;
		float radius;
		float height;
	public:
		Cylinder(Vector3 center, Vector3 axis, float radius, float height, const Material& material);

		//methods
		bool intersect(const Ray& ray, float& t) override;
		bool isWithinHeight(const Vector3& point) const;
		Vector3 getNormal(const Vector3& point) override;
};


class Triangle : public Shape {
	private:
		Vector3 v0;
		Vector3 v1;
		Vector3 v2;
	public:
		Triangle(Vector3 v0, Vector3 v1, Vector3 v2, const Material& material);

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