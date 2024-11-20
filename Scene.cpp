#include "Scene.h"

#include <iostream>


Scene::Scene(Color backgroundColor) : backgroundColor(backgroundColor){}
Scene::Scene(Color backgroundColor, std::vector<std::shared_ptr<Shape> > shapes, std::vector<std::shared_ptr<Light> > lights)
		: backgroundColor(backgroundColor), shapes(shapes), lights(lights) {}
Scene::~Scene(){}

void Scene::addShape(std::shared_ptr<Shape> shape){
	shapes.push_back(shape);
}
void Scene::addLight(std::shared_ptr<Light> light){
	lights.push_back(light);
}

bool Scene::intersect(const Ray& ray, float& t, bool limitDistance, float maxDistance){
	//Iterates over all shapes to find the closest intersection.
	float tmin = INFINITY;
	for (const std::shared_ptr<Shape>& shape : shapes){
		float tShape;	//distance t where the ray intersects the shape
		//if the ray intersects the shape and the intersection is closer than the previous one
		if (shape->intersect(ray, tShape) && tShape < tmin && ((limitDistance && tShape < maxDistance) || (!limitDistance))){
			//std::cout << "Intersection detected" << std::endl;
			tmin = tShape;
			lastHitObject = shape;
		}
	}
	t = tmin;
	return t < INFINITY;
}

bool Scene::isInShadow(const Vector3& intersectionPoint, const Vector3& lightDir, float lightDistance, const Vector3& surfaceNormal){
	float t;
	Ray shadowRay(intersectionPoint + surfaceNormal * 0.001f, lightDir);	//ray from intersection point (plus offset) to light source
	return intersect(shadowRay, t, true, lightDistance);
}

Color Scene::getBackgroundColor() const{
	return backgroundColor;
}

std::shared_ptr<Shape> Scene::getLastHitObject() const { return lastHitObject; }

std::vector<std::shared_ptr<Light> > Scene::getLights() const { return lights; }

void Scene::setBackgroundColor(Color color){
	backgroundColor = color;
}


