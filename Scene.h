#ifndef RAYTRACER_SCENE_H
#define RAYTRACER_SCENE_H
#include "Shape.h"
#include "Light.h"
#include "Color.h"
#include <vector>
#include <cmath>


class Scene {
	private:
		Color backgroundColor;
		/*std::vector<Sphere> spheres;
		std::vector<Cylinder> cylinders;
		std::vector<Triangle> triangles;*/
		std::vector<Shape> shapes;
		std::vector<PointLight> pointLights;

	public:
		Scene() = default;
		Scene(Color backgroundColor);
		Scene(Color backgroundColor, std::vector<Shape> shapes, std::vector<PointLight> pointLights);
		~Scene();
		void addSphere(Sphere sphere);
		void addCylinder(Cylinder cylinder);
		void addTriangle(Triangle triangle);
		void addPointLight(PointLight light);
		Shape intersect(const Ray& ray, float& t, bool limitDistance, float maxDistance, Shape hitObject);
		Color getBackgroundColor() const;
		std::vector<PointLight> getPointLights() const;
		void setBackgroundColor(Color color);
		bool isInShadow(const Vector3& intersectionPoint, const Vector3& lightDir, 
						float lightDistance, const Vector3& surfaceNormal, Shape hitObject);
};


#endif //RAYTRACER_SCENE_H
/* Scene
Attributes:
std::vector<Shape*> shapes;
std::vector<Light> lights;
Color backgroundColor;
Methods:
void addObject(Shape* shape);
void addLight(const Light& light);
bool intersect(const Ray& ray, float& t, Intersection& hit);
Iterates over all shapes to find the closest intersection.*/