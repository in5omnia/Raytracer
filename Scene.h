#ifndef RAYTRACER_SCENE_H
#define RAYTRACER_SCENE_H
#include "Shape.h"
#include "Light.h"
#include "Color.h"
#include "BVH.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>  // For formatted output (printTree) TODO: remove debug
#include <iostream>// For printTree TODO: remove debug


class Scene {
	private:
		Color backgroundColor;
		std::vector<Shape> allShapes;
		std::vector<PointLight> pointLights;
		std::shared_ptr<BVHNode> bvhRoot;

	public:
		Scene() = default;
		Scene(Color backgroundColor);
		Scene(Color backgroundColor, std::vector<Shape> shapes, std::vector<PointLight> pointLights);
		~Scene();
		void addSphere(Sphere sphere);
		void addCylinder(Cylinder cylinder);
		void addTriangle(Triangle triangle);
		void addPointLight(PointLight light);
		Color getBackgroundColor() const;
		std::vector<PointLight> getPointLights() const;
		std::vector<Shape>& getShapes();  // Return a reference
		std::shared_ptr<BVHNode> getBVHRoot() const;

		Shape intersectNodeShapes(const Ray& ray, float& t, bool limitDistance, float maxDistance, const std::vector<Shape>& nodeShapes);
		void setBackgroundColor(Color color);
		void setBVHRoot(std::shared_ptr<BVHNode> root);
		bool isInShadow(const Vector3& intersectionPoint, const Vector3& lightDir,
						float lightDistance, const Vector3& surfaceNormal);
		std::shared_ptr<BVHNode> buildBVH(std::vector<Shape>& shapes, int start, int end);
		Shape traverseBVH(const std::shared_ptr<BVHNode>& nodePtr,
						  const Ray& ray, float& tClosest, bool limitDistance, float maxDistance);
		void printTree(){
			//printTree
			std::cout << "Printing BVH Tree" << std::endl;
			printBVHNode(bvhRoot);
		}
		void printBVHNode(const std::shared_ptr<BVHNode>& node, int depth = 0) {
			if (!node) {
				std::cout << std::string(depth * 2, ' ') << "Null Node" << std::endl;
				return;
			}

			// Print node information
			const AABB& box = node->getBoundingBox();
			std::cout << std::string(depth * 2, ' ') << "Node (Depth: " << depth << ")\n";
			std::cout << std::string(depth * 2, ' ') << "  Bounding Box:\n";
			std::cout << std::string(depth * 2, ' ') << "    Min: ("
					  << box.getMin().x << ", " << box.getMin().y << ", " << box.getMin().z << ")\n";
			std::cout << std::string(depth * 2, ' ') << "    Max: ("
					  << box.getMax().x << ", " << box.getMax().y << ", " << box.getMax().z << ")\n";

			if (node->getIsLeaf()) {
				const auto& shapes = node->getShapes();
				std::cout << std::string(depth * 2, ' ') << "  Leaf Node with "
						  << shapes.size() << " shape(s)\n";
				for (const auto& shape : shapes) {
					// Assuming Shape class has a way to identify itself
					std::cout << std::string(depth * 2, ' ') << "    Shape: "
							  << shape.toString() << "\n";  // Replace `toString()` with your method
				}
			} else {
				std::cout << std::string(depth * 2, ' ') << "  Internal Node\n";
				std::cout << std::string(depth * 2, ' ') << "  Left Child:\n";
				printBVHNode(node->getLeftChild(), depth + 1);
				std::cout << std::string(depth * 2, ' ') << "  Right Child:\n";
				printBVHNode(node->getRightChild(), depth + 1);
			}
		}
};


#endif //RAYTRACER_SCENE_H
