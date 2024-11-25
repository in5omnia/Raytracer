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

		//methods
		Shape intersectNodeShapes(const Ray& ray, float& t, bool limitDistance,
								  float maxDistance, const std::vector<Shape>& nodeShapes);
		bool isInShadow(const Vector3& intersectionPoint, const Vector3& lightDir,
						float lightDistance, const Vector3& surfaceNormal);
		std::shared_ptr<BVHNode> buildBVH(std::vector<Shape>& shapes, int start, int end);
		Shape traverseBVH(const std::shared_ptr<BVHNode>& nodePtr,
						  const Ray& ray, float& tClosest, bool limitDistance, float maxDistance);

		//setters and adders
		void setBackgroundColor(Color color);
		void setBVHRoot(std::shared_ptr<BVHNode> root);
		void addSphere(Sphere sphere);
		void addCylinder(Cylinder cylinder);
		void addTriangle(Triangle triangle);
		void addPointLight(PointLight light);

		//getters
		Color getBackgroundColor() const;
		std::vector<PointLight> getPointLights() const;
		std::vector<Shape>& getShapes();  // Return a reference
		std::shared_ptr<BVHNode> getBVHRoot() const;

		//debug helpers
		void printTree();
		void printBVHNode(const std::shared_ptr<BVHNode>& node, int depth = 0);
};


#endif //RAYTRACER_SCENE_H
