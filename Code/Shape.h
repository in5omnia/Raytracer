#ifndef RAYTRACER_SHAPE_H
#define RAYTRACER_SHAPE_H
#include "Material.h"
#include "Ray.h"
#include "Vector3.h"
#include "Image.h"
#include "BVH.h"
#include <string>

#define SPHERE 0
#define CYLINDER 1
#define TRIANGLE 2
#define NO_SHAPE -1

class AABB;

class Sphere {
	private:
		Vector3 center;
		float radius;
		Material material;
	public:
		Sphere() = default;
		Sphere(Vector3 center, float radius, const Material& material);
		~Sphere() = default;

		//methods
		bool intersect(const Ray& ray, float& t);
		Vector3 getNormal(const Vector3& point);
		Material getMaterial() const;
		Color getTextureColor(const Vector3& point, const Image& texture);
		AABB getBoundingBox() const;
		std::string toString() const { return "Sphere"; }
};


class Cylinder {
	private:
		Vector3 center;
		Vector3 axis;
		float radius;
		float height;
		Material material;
	public:
		Cylinder() = default;
		Cylinder(Vector3 center, Vector3 axis, float radius, float height, const Material& material);
		~Cylinder() = default;

		//methods
		bool intersect(const Ray& ray, float& t);
		bool isWithinHeight(const Vector3& point) const;
		Vector3 getNormal(const Vector3& point);
		Material getMaterial() const;
		Color getTextureColor(const Vector3& point, const Image& texture);
		AABB getBoundingBox() const;
		std::string toString() const { return "Cylinder"; }
};


class Triangle {
	private:
		Vector3 v0;
		Vector3 v1;
		Vector3 v2;
		Material material;
	public:
		Triangle() = default;
		Triangle(Vector3 v0, Vector3 v1, Vector3 v2, const Material& material);
		~Triangle() = default;

		//methods
		bool intersect(const Ray& ray, float& t);
		Vector3 getNormal(const Vector3& rayDir);
		Material getMaterial() const;
		Color getTextureColor(const Vector3& point, const Image& texture);
		AABB getBoundingBox() const;
		std::string toString() const { return "Triangle"; }
};


class Shape {
	private:
		int shapeType; //0 for sphere, 1 for cylinder, 2 for triangle
		Sphere sphere;
		Cylinder cylinder;
		Triangle triangle;
	public:
		Shape(int shapeType);	//NO_SHAPE constructor
		Shape(Sphere sphere);
		Shape(Cylinder cylinder);
		Shape(Triangle triangle);

		//methods
		bool intersect(const Ray& ray, float& t);
		Vector3 getNormal(const Vector3& point);
		Material getMaterial() const;
		Color getTextureColor(const Vector3& point, const Image& texture);
		std::string toString() const;
		AABB getBoundingBox() const;
		int getShapeType() const;
};

#endif //RAYTRACER_SHAPE_H


