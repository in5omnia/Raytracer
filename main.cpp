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
		std::cout << "Starting image tests...\n\n";
		PinholeCamera camera(1200, 800, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f), 45.0f, 0.1f);
		//sphere test
		Scene scene(Color(0.0f, 0.0f, 0.0f));	//black background
		Vector3 center(-0.3, 0.19, 1);
		scene.addShape(std::make_shared<Sphere>(center, 0.2f, Material()));

		Raytracer raytracer(1, "binary", camera, scene);
		Image image = Image(1200, 800);
		raytracer.render(image);

		image.writePPM("binarySphere.ppm");
		return 0;

		// Test 1: Color gradient using float values
		/*std::cout << "Test 1: Creating color gradient (256x256)..." << std::endl;
		Image gradientImg(256, 256);

		for (int y = 0; y < gradientImg.getHeight(); y++) {
			for (int x = 0; x < gradientImg.getWidth(); x++) {
				float r = static_cast<float>(x) / gradientImg.getWidth();
				float g = static_cast<float>(y) / gradientImg.getHeight();
				float b = 0.5f;
				gradientImg.setPixelColorFloat(x, y, r, g, b);
			}
		}

		if (gradientImg.writePPM("gradient.ppm")) {
			std::cout << "✓ Successfully wrote gradient.ppm\n" << std::endl;
		} else {
			std::cerr << "✗ Failed to write gradient.ppm\n" << std::endl;
			return 1;
		}

		// Test 2: Basic shapes using integer values
		std::cout << "Test 2: Creating basic shapes (200x200)..." << std::endl;
		Image shapesImg(200, 200);

		// Black background
		for (int y = 0; y < shapesImg.getHeight(); y++) {
			for (int x = 0; x < shapesImg.getWidth(); x++) {
				shapesImg.setPixelColor(x, y, 0, 0, 0);
			}
		}

		// Red square
		for (int y = 50; y < 150; y++) {
			for (int x = 50; x < 150; x++) {
				shapesImg.setPixelColor(x, y, 255, 0, 0);
			}
		}

		// Green cross
		for (int i = 0; i < 200; i++) {
			// Horizontal line
			if (i < shapesImg.getWidth()) {
				shapesImg.setPixelColor(i, 100, 0, 255, 0);
			}
			// Vertical line
			if (i < shapesImg.getHeight()) {
				shapesImg.setPixelColor(100, i, 0, 255, 0);
			}
		}

		if (shapesImg.writePPM("shapes.ppm")) {
			std::cout << "✓ Successfully wrote shapes.ppm\n" << std::endl;
		} else {
			std::cerr << "✗ Failed to write shapes.ppm\n" << std::endl;
			return 1;
		}

		// Test 3: Bounds checking
		std::cout << "Test 3: Testing bounds checking..." << std::endl;
		try {
			shapesImg.setPixelColor(-1, 0, 0, 0, 0);
			std::cerr << "✗ Bounds checking failed: negative coordinate accepted\n" << std::endl;
			return 1;
		} catch (const std::out_of_range& e) {
			std::cout << "✓ Bounds checking caught negative coordinate\n" << std::endl;
		}

		try {
			shapesImg.setPixelColor(200, 0, 0, 0, 0);
			std::cerr << "✗ Bounds checking failed: out-of-bounds coordinate accepted\n" << std::endl;
			return 1;
		} catch (const std::out_of_range& e) {
			std::cout << "✓ Bounds checking caught out-of-bounds coordinate\n" << std::endl;
		}

		std::cout << "\nAll tests completed successfully!" << std::endl;
		std::cout << "Generated files:" << std::endl;
		std::cout << "1. gradient.ppm - Should show a red-green gradient with constant blue" << std::endl;
		std::cout << "2. shapes.ppm - Should show a red square with green cross on black background" << std::endl;

		return 0;

	} catch (const std::exception& e) {
		std::cerr << "\n✗ Unexpected error: " << e.what() << std::endl;
		return 1;
	}*/
}