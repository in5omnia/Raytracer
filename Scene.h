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
		std::vector<std::shared_ptr<Shape>> shapes;
		std::vector<std::shared_ptr<Light>> lights;

	public:
		Scene(Color backgroundColor);
		Scene(Color backgroundColor, std::vector<std::shared_ptr<Shape>> shapes, std::vector<std::shared_ptr<Light>> lights);
		~Scene();
		void addShape(std::shared_ptr<Shape> shape);
		void addLight(std::shared_ptr<Light> light);
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