#include "Scene.h"

#include <iostream>


Scene::Scene(Color backgroundColor) : backgroundColor(backgroundColor){}
Scene::Scene(Color backgroundColor, std::vector<Shape> shapes, std::vector<PointLight> lights)
		: backgroundColor(backgroundColor), shapes(shapes), pointLights(lights) {}
Scene::~Scene(){}

/*
void Scene::addSphere(Sphere sphere){
	spheres.push_back(sphere);
}
void Scene::addCylinder(Cylinder cylinder){
	cylinders.push_back(cylinder);
}
void Scene::addTriangle(Triangle triangle){
	triangles.push_back(triangle);
}
void Scene::addPointLight(PointLight pointLight){
	pointLights.push_back(pointLight);
}*/

void Scene::addSphere(Sphere sphere){
	Shape shape = Shape(sphere);
	shapes.push_back(shape);
}
void Scene::addCylinder(Cylinder cylinder){
	Shape shape = Shape(cylinder);
	shapes.push_back(shape);
}
void Scene::addTriangle(Triangle triangle){
	Shape shape = Shape(triangle);
	shapes.push_back(shape);
}
void Scene::addPointLight(PointLight pointLight){
	pointLights.push_back(pointLight);
}


Shape Scene::intersect(const Ray& ray, float& t, bool limitDistance, float maxDistance, Shape hitObject){
	//Iterates over all shapes to find the closest intersection.
	float tmin = INFINITY;
	Shape lastHitObject = Shape(NO_SHAPE);

	for (Shape shape : shapes){
		float tShape;	//distance t where the ray intersects the shape
		//if the ray intersects the shape and the intersection is closer than the previous one
		if (shape.intersect(ray, tShape) && tShape < tmin && ((limitDistance && tShape < maxDistance /*&& hitObject.getShape() != shape*/) || (!limitDistance))){
			//DEBUG Does this work? hitObject != shape ;
			tmin = tShape;
			lastHitObject = shape;
		}
	}
	t = tmin;
	if (t < INFINITY)	return lastHitObject;
	else				return Shape(NO_SHAPE);	//no intersection 
	//TODO: is this necessary? Cant i just return always lathitobject and check?
	//return t < INFINITY;
}

bool Scene::isInShadow(const Vector3& intersectionPoint, const Vector3& lightDir,
					   float lightDistance, const Vector3& surfaceNormal, Shape hitObject){
	float t;
	float offset = 0.001f;
	Ray shadowRay(intersectionPoint + surfaceNormal * offset, lightDir);	//ray from intersection point (plus offset) to light source
	return (intersect(shadowRay, t, true, lightDistance, hitObject).getShapeType() != NO_SHAPE);
}

Color Scene::getBackgroundColor() const{
	return backgroundColor;
}


std::vector<PointLight> Scene::getPointLights() const { return pointLights; }

void Scene::setBackgroundColor(Color color){
	backgroundColor = color;
}


