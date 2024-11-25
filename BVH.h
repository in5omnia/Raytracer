#ifndef RAYTRACER_BVH_H
#define RAYTRACER_BVH_H

#include "Shape.h"
#include "Vector3.h"
#include "Ray.h"
#include <memory>

#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2

class Shape;

class AABB {
	private:
		Vector3 min, max;
	public:
		AABB() = default;
		AABB(const Vector3& min, const Vector3& max);

		bool intersect(const Ray& ray, float& tMin, float& tMax) const;

		static AABB surroundingBox(const AABB& box1, const AABB& box2);

		int getLongestAxis() const;

		Vector3 centroid() const;

		Vector3 getMin() const;
		Vector3 getMax() const;
};




class BVHNode {
	private:
		AABB boundingBox;                         // The bounding volume of this node
		std::shared_ptr<BVHNode> leftChild = nullptr;       // Pointer to the left child node
		std::shared_ptr<BVHNode> rightChild = nullptr;      // Pointer to the right child node
		std::vector<Shape> shapes;                // Shapes in this node if it's a leaf
		bool isLeaf = false;                 // Indicates if this is a leaf node
	public:
		BVHNode() = default;
		// Constructor for internal nodes
		BVHNode(const AABB& box, std::shared_ptr<BVHNode> _leftChild, std::shared_ptr<BVHNode> _rightChild);

		// Constructor for leaf nodes
		BVHNode(const AABB& box, const std::vector<Shape>& shapes);

		void setLeftChild(std::shared_ptr<BVHNode> node);
		void setRightChild(std::shared_ptr<BVHNode> node);
		void addShape(Shape shape);
		std::vector<Shape> getShapes() const;
		std::shared_ptr<BVHNode> getLeftChild() const;
		std::shared_ptr<BVHNode> getRightChild() const;
		bool getIsLeaf() const;
		AABB getBoundingBox() const;
};


/*
class BVHNode {
private:
	AABB boundingBox;           // The bounding volume of this node
	int leftChildIndex = -1;    // Index of the left child (-1 if no child)
	int rightChildIndex = -1;   // Index of the right child (-1 if no child)
	std::vector<Shape> shapes;  // Shapes in this node if it's a leaf
	bool isLeaf = false;        // Indicates if this is a leaf node

public:
	BVHNode() = default;

	// Constructor for leaf nodes
	BVHNode(const AABB& box, const std::vector<Shape>& shapes);

	// Constructor for internal nodes
	BVHNode(const AABB& box, int left, int right);

	// Accessors
	bool getIsLeaf() const ;
	const AABB& getBoundingBox() const;
	const std::vector<Shape>& getShapes() const;
	int getLeftChildIndex() const;
	int getRightChildIndex() const;

	void setLeftChildIndex(int index);
	void setRightChildIndex(int index);
	void addShape(Shape shape);
};


class BVHTree {
private:
	std::vector<BVHNode> nodes;  // Stores all BVH nodes
	int rootIndex = -1;          // Index of the root node

public:
	BVHTree() = default;

	int addNode(const BVHNode& node) {
		nodes.push_back(node);
		return nodes.size() - 1;  // Return the index of the newly added node
	}

	const BVHNode& getNode(int index) const {
		return nodes[index];
	}

	void setRootIndex(int index) {
		rootIndex = index;
	}

	int getRootIndex() const {
		return rootIndex;
	}
};

*/
#endif //RAYTRACER_BVH_H
