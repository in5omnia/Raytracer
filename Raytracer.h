#ifndef RAYTRACER_RAYTRACER_H
#define RAYTRACER_RAYTRACER_H
//#include <nlohmann/json.hpp>  // JSON library
#include "json.hpp"
#include <fstream>
#include "Image.h"
#include "Scene.h"
#include "Camera.h"
#include "Ray.h"
#include "Color.h"
#include "Raytracer.h"
#include "Scene.h"
#include "Shape.h"
#include "Light.h"
#include "Material.h"

class Raytracer {
	private:
		int nbounces;
		std::string rendermode;
		PinholeCamera camera;
		Scene scene;

	public:
		Raytracer(int nbounces,
				  std::string rendermode,
				  PinholeCamera& camera,
				  Scene& scene
				  );
		void render(Image& image);
		Color traceRay(const Ray& ray);
		Color shadeBlinnPhong(const Ray& ray, float& t);

		//read json method
		void readJSON(const std::string& filename);

};


#endif //RAYTRACER_RAYTRACER_H
/*Attributes:
Reference to the Scene.
Reference to the Camera.
Maximum recursion depth (nbounces).
Methods:
void render(Image& image); -> Main rendering loop.
Color traceRay(const Ray& ray, int depth); -> Recursive ray tracing method.*/