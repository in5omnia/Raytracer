#ifndef RAYTRACER_RAYTRACER_H
#define RAYTRACER_RAYTRACER_H

#include "Image.h"
#include "Scene.h"
#include "Camera.h"
#include "Ray.h"
#include "Color.h"

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

		//read json method

};


#endif //RAYTRACER_RAYTRACER_H
/*Attributes:
Reference to the Scene.
Reference to the Camera.
Maximum recursion depth (nbounces).
Methods:
void render(Image& image); -> Main rendering loop.
Color traceRay(const Ray& ray, int depth); -> Recursive ray tracing method.*/