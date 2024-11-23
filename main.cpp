#include <iostream>
#include "Image.h"
#include "Scene.h"
#include "Shape.h"
#include "Color.h"
#include "Material.h"
#include "Vector3.h"
#include "Camera.h"
#include "Raytracer.h"
#include <omp.h>

int main() {
	double time;
	
	Raytracer raytracer = Raytracer();
	Image image = raytracer.readJSON("jsons/scenePhong.json");

	time = omp_get_wtime();
	raytracer.render(image);

	time = omp_get_wtime() - time;
	std::cout << "Time: " << time << "s" << std::endl;
	image.writePPM("blinnPhong.ppm");
	return 0;

}