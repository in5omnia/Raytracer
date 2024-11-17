#include "Raytracer.h"

Raytracer::Raytracer(int nbounces,
		  std::string rendermode,
		  PinholeCamera& camera,
		  Scene& scene
) :  nbounces(nbounces), rendermode(rendermode), camera(camera), scene(scene) {}


void Raytracer::render(Image& image) {
	int width = image.getWidth();
	int height = image.getHeight();

	for (int y = 0; y < height; ++y) {		//bottom to top
		for (int x = 0; x < width; ++x) {	//left to right
			//Normalized pixel coordinates
			float u = (static_cast<float>(x) + 0.5f) / static_cast<float>(width);
			float v = (static_cast<float>(y) + 0.5f) / static_cast<float>(height);
			v = 1.0f - v; // Flip v if necessary

			Ray ray = camera.generateRay(u, v);
			Color color = traceRay(ray);

			image.setPixelColor(x, y, color);
		}
	}
}

Color Raytracer::traceRay(const Ray& ray) {
	// TODO: Implement ray tracing logic here
	//  + consider the number of bounces

	if (rendermode == "binary"){
		float t;  // Distance to the closest intersection
		if (scene.intersect(ray, t)) {
			// If an intersection occurs, return red for the object
			return Color(1.0f, 0.0f, 0.0f);  // Red color
		}

		// No intersection: return black for background
		return Color(0.0f, 0.0f, 0.0f);  // Black color
	}
	else if (rendermode == "phong") {
		//TODO: implement phong shading
	}

	if (nbounces == 0) {std::cout << "REMOVE ME" << std::endl;} //TODO: remove this line (compiler fix)
	return Color();	//TODO: remove this line
	/*
	Should handle:
	- Intersection tests with all objects in the scene. -> Scene::intersect()
	- Shading calculations using the Blinn-Phong model.
	- Handling reflections and refractions if implemented.
	- Background color if no intersection occurs.
	*/
}


