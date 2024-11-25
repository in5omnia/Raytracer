#include "BVH.h"

/* AABB class methods */

AABB::AABB(const Vector3& min, const Vector3& max) : min(min), max(max) {}

bool AABB::intersect(const Ray& ray, float& tMin, float& tMax) const {
	tMin = -INFINITY;
	tMax = INFINITY;

	for (int i = 0; i < 3; ++i) {
		float invD = 1.0f / (ray.getDirection()[i] + 1e-8f);  // Avoid division by zero
		float t0 = (min[i] - ray.getOrigin()[i]) * invD;
		float t1 = (max[i] - ray.getOrigin()[i]) * invD;

		if (invD < 0.0f) std::swap(t0, t1);

		tMin = std::max(tMin, t0);
		tMax = std::min(tMax, t1);

		if (tMax <= tMin) return false;  // Early exit if ranges don't overlap
	}

	return true;
}

AABB AABB::surroundingBox(const AABB& box1, const AABB& box2) {
	Vector3 small(std::min(box1.min.x, box2.min.x),
				  std::min(box1.min.y, box2.min.y),
				  std::min(box1.min.z, box2.min.z));
	Vector3 big(std::max(box1.max.x, box2.max.x),
				std::max(box1.max.y, box2.max.y),
				std::max(box1.max.z, box2.max.z));

	// Ensure non-degenerate bounding box by adding a small epsilon if needed
	if (big.x - small.x < 1e-6f) big.x = small.x + 1e-6f;
	if (big.y - small.y < 1e-6f) big.y = small.y + 1e-6f;
	if (big.z - small.z < 1e-6f) big.z = small.z + 1e-6f;

	return AABB(small, big);
}

int AABB::getLongestAxis() const {
	Vector3 diagonal = max - min;
	if (diagonal.x > diagonal.y && diagonal.x > diagonal.z) {
		return 0;
	} else if (diagonal.y > diagonal.z) {
		return 1;
	} else {
		return 2;
	}
}

Vector3 AABB::centroid() const {
	return (min + max) * 0.5f;  // Element-wise average of min and max
}

Vector3 AABB::getMin() const {
	return min;  // Element-wise average of min and max
}

Vector3 AABB::getMax() const {
	return max;  // Element-wise average of min and max
}

/* BVHNode class methods */

// Constructor for internal nodes
BVHNode::BVHNode(const AABB& box, std::shared_ptr<BVHNode> _leftChild, std::shared_ptr<BVHNode> _rightChild)
		: boundingBox(box), leftChild(_leftChild), rightChild(_rightChild), isLeaf(false) {}

// Constructor for leaf nodes
BVHNode::BVHNode(const AABB& box, const std::vector<Shape>& shapes)
		: boundingBox(box), shapes(shapes), isLeaf(true) {}

//setters and adders
void BVHNode::setLeftChild(std::shared_ptr<BVHNode> left) {
	leftChild = left;
}

void BVHNode::setRightChild(std::shared_ptr<BVHNode> right) {
	rightChild = right;
}

void BVHNode::addShape(Shape shape){
	shapes.push_back(shape);
}

//getters
std::vector<Shape> BVHNode::getShapes() const{
	return shapes;
}

std::shared_ptr<BVHNode> BVHNode::getLeftChild() const {
	return leftChild;
}

std::shared_ptr<BVHNode> BVHNode::getRightChild() const {
	return rightChild;
}

bool BVHNode::getIsLeaf() const {
	return isLeaf;
}

AABB BVHNode::getBoundingBox() const {
	return boundingBox;
}


