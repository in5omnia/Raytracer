#include "Scene.h"

Scene::Scene(Color backgroundColor) : backgroundColor(backgroundColor){}
Scene::Scene(Color backgroundColor, std::vector<std::unique_ptr<Shape> > shapes, std::vector<std::unique_ptr<Light> > lights)
		: backgroundColor(backgroundColor), shapes(std::move(shapes)), lights(std::move(lights)) {}
Scene::~Scene(){}

void Scene::addShape(std::unique_ptr<Shape> shape){
	shapes.push_back(std::move(shape));
}
void Scene::addLight(std::unique_ptr<Light> light){
	lights.push_back(std::move(light));
}

bool Scene::intersect(const Ray& ray, float& t){
	//Iterates over all shapes to find the closest intersection.
	float tmin = INFINITY;
	for (const std::unique_ptr<Shape>& shape : shapes){
		float tShape;	//point t where the ray intersects the shape
		//if the ray intersects the shape and the intersection is closer than the previous one
		if (shape->intersect(ray, tShape) && tShape < tmin){
			tmin = tShape;
		}
	}
	t = tmin;
	return t < INFINITY;
}
