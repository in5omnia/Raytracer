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

std::shared_ptr<Shape> Scene::intersect(const Ray& ray, float& t, bool limitDistance, float maxDistance, std::shared_ptr<Shape> hitObject){
	//Iterates over all shapes to find the closest intersection.
	float tmin = INFINITY;
	std::shared_ptr<Shape> lastHitObject = nullptr;

	for (const std::shared_ptr<Shape>& shape : shapes){
		float tShape;	//distance t where the ray intersects the shape
		//if the ray intersects the shape and the intersection is closer than the previous one
		if (shape->intersect(ray, tShape) && tShape < tmin && ((limitDistance && tShape < maxDistance /*&& hitObject != shape*/) || (!limitDistance))){
			//DEBUG Does this work? hitObject != shape ;
			tmin = tShape;
			lastHitObject = shape;
		}
	}
	t = tmin;
	if (t < INFINITY)	return lastHitObject;
	else				return nullptr;
	//return t < INFINITY;
}

bool Scene::isInShadow(const Vector3& intersectionPoint, const Vector3& lightDir,
					   float lightDistance, const Vector3& surfaceNormal, std::shared_ptr<Shape> hitObject){
	float t;
	float offset = 0.001f;
	Ray shadowRay(intersectionPoint + surfaceNormal * offset, lightDir);	//ray from intersection point (plus offset) to light source
	return (intersect(shadowRay, t, true, lightDistance, hitObject) != nullptr);
}

Color Scene::getBackgroundColor() const{
	return backgroundColor;
}

//std::shared_ptr<Shape> Scene::getLastHitObject() const { return lastHitObject; }

std::vector<std::shared_ptr<Light> > Scene::getLights() const { return lights; }

void Scene::setBackgroundColor(Color color){
	backgroundColor = color;
}


