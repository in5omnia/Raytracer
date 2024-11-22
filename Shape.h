#ifndef RAYTRACER_SHAPE_H
#define RAYTRACER_SHAPE_H
#include "Material.h"
#include "Ray.h"
#include "Vector3.h"
#include <string>


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
		virtual std::string toString() const = 0;
		virtual Vector3 getV0() = 0;	//DEBUG TODO: remove
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
		std::string toString() const override { return "Sphere"; }
		Vector3 getV0() override { return 0; }	//DEBUG TODO: remove
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
		std::string toString() const override { return "Cylinder"; }
	 	Vector3 getV0() override { return 0; }	//DEBUG TODO: remove
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
		Vector3 getNormal(const Vector3& rayDir) override;
		std::string toString() const override { return "Triangle"; }
		Vector3 getV0() override { return v0; }	//DEBUG TODO: remove
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