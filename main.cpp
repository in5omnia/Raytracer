#include <iostream>
#include "Image.h"
#include "Scene.h"
#include "Shape.h"
#include "Color.h"
#include "Material.h"
#include "Vector3.h"
#include "Camera.h"
#include "Raytracer.h"


void readJSON(const std::string& filename);	//will return a scene object


int main() {
	//try {
		/*std::cout << "Starting image tests...\n\n";
		PinholeCamera camera(1200, 800, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f), 45.0f, 0.1f);
		//sphere test
		Scene scene(Color(0.0f, 0.0f, 0.0f));	//black background
		Vector3 center(-0.3, 0.19, 1);
		scene.addShape(std::make_shared<Sphere>(center, 0.2f, Material()));
		//cylinder test
		Vector3 cylinderCenter(-0.3, -0.2, 1);
		Vector3 axis(1, 0, 0);
		scene.addShape(std::make_shared<Cylinder>(cylinderCenter, axis, 0.15f, 0.2f, Material()));
		//triangle test
		Vector3 v0(0, 0, 1);
		Vector3 v1(0.5, 0, 1);
		Vector3 v2(0.25, 0.25, 1);
		scene.addShape(std::make_shared<Triangle>(v0, v1, v2, Material()));

		Raytracer raytracer(1, "binary", camera, scene);
		Image image = Image(1200, 800);
		raytracer.render(image);
		image.writePPM("testingBinary.ppm");*/


		Raytracer raytracer = Raytracer();
		Image image = raytracer.readJSON("jsons/simple_phong.json");
		raytracer.render(image);
		image.writePPM("blinnPhong.ppm");
		return 0;

}