#include "Scene.h"

Scene::Scene(Color backgroundColor) : backgroundColor(backgroundColor){}

Scene::Scene(Color backgroundColor,
			 std::vector<Shape> shapes,
			 std::vector<PointLight> lights)
		: backgroundColor(backgroundColor),
		allShapes(shapes), pointLights(lights) {}

Scene::~Scene(){}

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

std::shared_ptr<BVHNode> Scene::getBVHRoot() const {
	return bvhRoot;
}

std::vector<Shape>& Scene::getShapes() {
	return allShapes;
}

Color Scene::getBackgroundColor() const{
	return backgroundColor;
}

std::vector<PointLight> Scene::getPointLights() const {
	return pointLights;
}

void Scene::setBackgroundColor(Color color){
	backgroundColor = color;
}

void Scene::setBVHRoot(std::shared_ptr<BVHNode> root){
	bvhRoot = root;
}

void Scene::printTree(){
	std::cout << "Printing BVH Tree" << std::endl;
	printBVHNode(bvhRoot);
}

void Scene::printBVHNode(const std::shared_ptr<BVHNode>& node, int depth) {
	if (!node) {
		std::cout << std::string(depth * 2, ' ') << "Null Node" << std::endl;
		return;
	}

	// Print node information
	const AABB& box = node->getBoundingBox();
	std::cout << std::string(depth * 2, ' ')
			  << "Node (Depth: " << depth << ")\n";
	std::cout << std::string(depth * 2, ' ') << "  Bounding Box:\n";
	std::cout << std::string(depth * 2, ' ') << "    Min: ("
			  << box.getMin().x << ", " << box.getMin().y
			  << ", " << box.getMin().z << ")\n";
	std::cout << std::string(depth * 2, ' ') << "    Max: ("
			  << box.getMax().x << ", " << box.getMax().y << ", "
			  << box.getMax().z << ")\n";

	if (node->getIsLeaf()) {
		const auto& shapes = node->getShapes();
		std::cout << std::string(depth * 2, ' ')
				  << "  Leaf Node with " << shapes.size()
				  << " shape(s)\n";
		for (const auto& shape : shapes) {
			std::cout << std::string(depth * 2, ' ')
					  << "    Shape: "
					  << shape.toString() << "\n";
		}
	} else {
		std::cout << std::string(depth * 2, ' ') << "  Internal Node\n";
		std::cout << std::string(depth * 2, ' ') << "  Left Child:\n";
		printBVHNode(node->getLeftChild(), depth + 1);
		std::cout << std::string(depth * 2, ' ') << "  Right Child:\n";
		printBVHNode(node->getRightChild(), depth + 1);
	}
}

Shape Scene::intersectNodeShapes(const Ray& ray, float& t,
								 bool limitDistance, float maxDistance,
								 const std::vector<Shape>& nodeShapes){
	// Iterates over all shapes to find the closest intersection.
	float tmin = INFINITY;
	// default shape representing no intersection
	Shape lastHitObject = Shape(NO_SHAPE);

	for (Shape shape : nodeShapes){
		float tShape;	// distance t where the ray intersects the shape
		// If there's an intersection closer than the previous one,
		// save shape and distance
		if (shape.intersect(ray, tShape) && tShape < tmin
			&& ((limitDistance && tShape < maxDistance) || (!limitDistance))){
			tmin = tShape;
			lastHitObject = shape;
		}
	}
	t = tmin;
	return lastHitObject;
}

bool Scene::isInShadow(const Vector3& intersectionPoint,
					   const Vector3& lightDir,
					   float lightDistance,
					   const Vector3& surfaceNormal){
	float t;
	float offset = 0.001f;
	// ray from intersection point (plus offset) to light source
	Ray shadowRay(intersectionPoint + surfaceNormal * offset, lightDir);
	return (traverseBVH(bvhRoot, shadowRay, t, true, lightDistance)
			.getShapeType() != NO_SHAPE);
}

std::shared_ptr<BVHNode> Scene::buildBVH(std::vector<Shape>& shapes,
										 int start, int end) {
	// Compute the bounding box surrounding all shapes in this range
	AABB boundingBox = shapes[start].getBoundingBox();
	for (int i = start + 1; i < end; ++i) {
		boundingBox = AABB::surroundingBox(boundingBox,
										   shapes[i].getBoundingBox());
	}

	int shapeCount = end - start;
	// Leaf node: store the shapes directly
	if (shapeCount <= 2) {
		return std::make_shared<BVHNode>(boundingBox,
	 	std::vector<Shape>(shapes.begin() + start, shapes.begin() + end));
	}

	// Choose axis to split (longest axis)
	int axis = boundingBox.getLongestAxis();
	std::sort(shapes.begin() + start, shapes.begin() + end,
			  [axis](const Shape& a, const Shape& b) {
				  return a.getBoundingBox().centroid()[axis]
				  < b.getBoundingBox().centroid()[axis];
			  });

	int mid = start + shapeCount / 2;

	// Recursively build left and right child nodes
	auto leftChild = buildBVH(shapes, start, mid);
	auto rightChild = buildBVH(shapes, mid, end);

	// Internal node: store child nodes
	return std::make_shared<BVHNode>(boundingBox, leftChild, rightChild);
}

Shape Scene::traverseBVH(const std::shared_ptr<BVHNode>& nodePtr,
						 const Ray& ray, float& tClosest,
						 bool limitDistance, float maxDistance) {
	const BVHNode& node = *nodePtr;
	float tMin = 0.0f, tMax = tClosest;

	if (!node.getBoundingBox().intersect(ray, tMin, tMax)) {
		// Ray misses the bounding box (no intersection)
		return Shape(NO_SHAPE);;
	}

	// Leaf node: test intersection with all its shapes
	if (node.getIsLeaf()) {
		Shape hit = intersectNodeShapes(ray, tClosest, limitDistance,
										maxDistance, node.getShapes());
		return hit;
	}
	float tLeft = 0.0f, tRight = 0.0f;
	// Internal node: traverse children
	Shape hitLeft = traverseBVH(node.getLeftChild(), ray, tLeft,
								limitDistance, maxDistance);
	Shape hitRight = traverseBVH(node.getRightChild(), ray, tRight,
								 limitDistance, maxDistance);

	// Determine which child provided the closest intersection
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