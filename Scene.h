#ifndef RAYTRACER_SCENE_H
#define RAYTRACER_SCENE_H
#include "Shape.h"
#include "Light.h"
#include "Color.h"
#include <vector>
#include <cmath>


class Scene {
	private:
		std::vector<std::unique_ptr<Shape>> shapes;
		std::vector<std::unique_ptr<Light>> lights;
		Color backgroundColor;

	public:
		Scene(Color backgroundColor);
		Scene(Color backgroundColor, std::vector<std::unique_ptr<Shape>> shapes, std::vector<std::unique_ptr<Light>> lights);
		~Scene();
		void addShape(std::unique_ptr<Shape> shape);
		void addLight(std::unique_ptr<Light> light);
		bool intersect(const Ray& ray, float& t);
		//Iterates over all shapes to find the closest intersection.
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