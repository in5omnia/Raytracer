#ifndef RAYTRACER_RAYTRACER_H
#define RAYTRACER_RAYTRACER_H
#include "json.hpp"
#include <fstream>
#include "Image.h"
#include "Scene.h"
#include "Camera.h"
#include "Ray.h"
#include "Color.h"
#include "Scene.h"
#include "Shape.h"
#include "Light.h"
#include "Material.h"

#define Ka 0.2f

class Raytracer {
	private:
		int nbounces;
		std::string rendermode;
		std::shared_ptr<Camera> camera = nullptr;
		Scene scene;

	public:
		Raytracer();
		void render(Image& image);
		Color traceRay(const Ray& ray, int depth);
		Color shadeBlinnPhong(const Ray& ray, float& t, std::shared_ptr<Shape> hitObject);

		//read json method
		Image readJSON(const std::string& filename);

};


#endif //RAYTRACER_RAYTRACER_H
/*Attributes:
Reference to the Scene.
Reference to the Camera.
Maximum recursion depth (nbounces).
Methods:
void render(Image& image); -> Main rendering loop.
Color traceRay(const Ray& ray, int depth); -> Recursive ray tracing method.*/