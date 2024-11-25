#include "Scene.h"

#include <iostream>


Scene::Scene(Color backgroundColor) : backgroundColor(backgroundColor){}
Scene::Scene(Color backgroundColor, std::vector<Shape> shapes, std::vector<PointLight> lights)
		: backgroundColor(backgroundColor), allShapes(shapes), pointLights(lights) {}
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
	allShapes.push_back(shape);
}
void Scene::addCylinder(Cylinder cylinder){
	Shape shape = Shape(cylinder);
	allShapes.push_back(shape);
}
void Scene::addTriangle(Triangle triangle){
	Shape shape = Shape(triangle);
	allShapes.push_back(shape);
}
void Scene::addPointLight(PointLight pointLight){
	pointLights.push_back(pointLight);
}

std::shared_ptr<BVHNode> Scene::getBVHRoot() const { return bvhRoot; }

std::vector<Shape>& Scene::getShapes() { return allShapes; }


Shape Scene::intersectNodeShapes(const Ray& ray, float& t, bool limitDistance, float maxDistance, const std::vector<Shape>& nodeShapes){
	//Iterates over all shapes to find the closest intersection.
	float tmin = INFINITY;
	Shape lastHitObject = Shape(NO_SHAPE);	// default shape representing no intersection

	for (Shape shape : nodeShapes){
		float tShape;	//distance t where the ray intersects the shape
		//if the ray intersects the shape and the intersection is closer than the previous one
		if (shape.intersect(ray, tShape) && tShape < tmin && ((limitDistance && tShape < maxDistance) || (!limitDistance))){
			tmin = tShape;
			lastHitObject = shape;
		}
	}
	t = tmin;
	return lastHitObject;
}

bool Scene::isInShadow(const Vector3& intersectionPoint, const Vector3& lightDir,
					   float lightDistance, const Vector3& surfaceNormal){
	float t;
	float offset = 0.001f;
	Ray shadowRay(intersectionPoint + surfaceNormal * offset, lightDir);	//ray from intersection point (plus offset) to light source
	return (traverseBVH(bvhRoot, shadowRay, t, true, lightDistance).getShapeType() != NO_SHAPE);
}

Color Scene::getBackgroundColor() const{
	return backgroundColor;
}


std::vector<PointLight> Scene::getPointLights() const { return pointLights; }

void Scene::setBackgroundColor(Color color){
	backgroundColor = color;
}

void Scene::setBVHRoot(std::shared_ptr<BVHNode> root){
	bvhRoot = root;
}


std::shared_ptr<BVHNode> Scene::buildBVH(std::vector<Shape>& shapes, int start, int end) {
	// Compute the bounding box surrounding all shapes in this range
	AABB boundingBox = shapes[start].getBoundingBox();
	for (int i = start + 1; i < end; ++i) {
		boundingBox = AABB::surroundingBox(boundingBox, shapes[i].getBoundingBox());
	}

	int shapeCount = end - start;
	// Leaf node: store the shapes directly
	if (shapeCount <= 2) {
		return std::make_unique<BVHNode>(boundingBox, std::vector<Shape>(shapes.begin() + start, shapes.begin() + end));
		//return BVHNode(bounds, std::vector<Shape>(shapes.begin() + start, shapes.begin() + end));
	}

	// Choose axis to split (longest axis)
	int axis = boundingBox.getLongestAxis();
	std::sort(shapes.begin() + start, shapes.begin() + end,
			  [axis](const Shape& a, const Shape& b) {
				  return a.getBoundingBox().centroid()[axis] < b.getBoundingBox().centroid()[axis];
			  });

	int mid = start + shapeCount / 2;

	// Recursively build left and right child nodes
	auto leftChild = buildBVH(shapes, start, mid);
	auto rightChild = buildBVH(shapes, mid, end);

	// Internal node: store child nodes
	return std::make_shared<BVHNode>(boundingBox, leftChild, rightChild);
}

Shape Scene::traverseBVH(const std::shared_ptr<BVHNode>& nodePtr, const Ray& ray, float& tClosest, bool limitDistance, float maxDistance) {
	const BVHNode& node = *nodePtr;
	float tMin = 0.0f, tMax = tClosest;

	if (!node.getBoundingBox().intersect(ray, tMin, tMax)) {
		return Shape(NO_SHAPE);;  // Ray misses the bounding box (no intersection)
	}

	// Leaf node: test intersection with all its shapes
	if (node.getIsLeaf()) {
		Shape hit = intersectNodeShapes(ray, tClosest, limitDistance, maxDistance, node.getShapes());
		return hit;
	}
	float tLeft = 0.0f, tRight = 0.0f;
	// Internal node: traverse children
	Shape hitLeft = traverseBVH(node.getLeftChild(), ray, tLeft, limitDistance, maxDistance);
	Shape hitRight = traverseBVH(node.getRightChild(), ray, tRight, limitDistance, maxDistance);

	// 4. Determine which child provided the closest intersection
	if (hitLeft.getShapeType() != NO_SHAPE) {
		if (hitRight.getShapeType() != NO_SHAPE){
			// Both branches have hits; choose the closer one
			if (tLeft < tRight) {
				tClosest = tLeft;
				return hitLeft;
			}
			tClosest = tRight;
			return hitRight;
		}
		// Only the left branch has a hit
		tClosest = tLeft;
		return hitLeft;

	} else if (hitRight.getShapeType() != NO_SHAPE) {
		// Only the right branch has a hit
		tClosest = tRight;
		return hitRight;
	}
	// No hits in either branch
	return Shape(NO_SHAPE);
}