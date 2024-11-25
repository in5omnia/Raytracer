#ifndef RAYTRACER_RAYTRACER_H
#define RAYTRACER_RAYTRACER_H
#include "json.hpp"
#include <fstream>
#include <cstdlib>
//#include <omp.h>
#include <stack>  // Include this at the top of your file
#include "Image.h"
#include "Scene.h"
#include "Camera.h"
#include "Ray.h"
#include "Color.h"
#include "Scene.h"
#include "Shape.h"
#include "Light.h"
#include "Material.h"
#include "BVH.h"


#define Ka 0.2f

class Raytracer {
	private:
		int nbounces;
		int numPixelSamples;
		int numApertureSamples;
		std::string rendermode;
	Camera camera;
		Scene scene;

	public:
		Raytracer();
		void render(Image& image);
		void renderWithApertureSampling(Image& image);
		Color traceRay(const Ray& ray, int depth, std::stack<float> refractiveStack);
		Color shadeBlinnPhong(const Ray& ray, float& t, Shape hitObject);

		//read json method
		Image readJSON(const std::string& filename);

};


#endif //RAYTRACER_RAYTRACER_H
