#include "Image.h"
#include <fstream>
#include <stdexcept>
//TODO: maybe replace r,g, b with a single color struct = Vector3 (replace in .h too)

Image::Image(int w, int h) : width(w), height(h) {
	pixels.resize(width * height);
}

/*void Image::setPixelColor(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
	if (x < 0 || x >= width || y < 0 || y >= height) {
		throw std::out_of_range("Pixel coordinates out of bounds");
	}

	//int index = (y * width + x) * 3;
	//pixels[index] = r;
	//pixels[index + 1] = g;
	//pixels[index + 2] = b;
	int index = (y * width + x);
	pixels[index] = Color(r, g, b);
}*/

void Image::setPixelColor(int x, int y, Color color) {
	if (x < 0 || x >= width || y < 0 || y >= height) {
		throw std::out_of_range("Pixel coordinates out of bounds");
	}

	int index = (y * width + x);
	pixels[index] = color;
}

/*void Image::setPixelColorFloat(int x, int y, float r, float g, float b) {
	setPixelColor(x, y,
				  static_cast<uint8_t>(r * 255),
				  static_cast<uint8_t>(g * 255),
				  static_cast<uint8_t>(b * 255));
}*/

bool Image::writePPM(const std::string& filename) const {
	std::ofstream file(filename, std::ios::binary);
	if (!file) {
		return false;
	}

	file << "P6\n";
	file << width << " " << height << "\n";
	file << "255\n";

	// Write pixel data
	for (const Color& color : pixels) {
		file.put(static_cast<char>(color.getR()*255));  // Red channel
		file.put(static_cast<char>(color.getG()*255));  // Green channel
		file.put(static_cast<char>(color.getB()*255));  // Blue channel
	}
	//file.write(reinterpret_cast<const char*>(pixels.data()), pixels.size());

	return file.good();
}

int Image::getWidth() const { return width; }
int Image::getHeight() const { return height; }