#include "Image.h"

Image::Image() : width(0), height(0) {}

Image::Image(int w, int h) : width(w), height(h) {
	pixels.resize(width * height);
}

void Image::setPixelColor(int x, int y, Color color) {
	if (x < 0 || x >= width || y < 0 || y >= height) {
		throw std::out_of_range("Pixel coordinates out of bounds");
	}

	int index = (y * width + x);
	pixels[index] = color;
}

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
		file.put(static_cast<char>(color.getR() * 255));  // Red channel
		file.put(static_cast<char>(color.getG() * 255));  // Green channel
		file.put(static_cast<char>(color.getB() * 255));  // Blue channel
	}

	return file.good();
}

int Image::getWidth() const { return width; }
int Image::getHeight() const { return height; }

Image::Image(const std::string& filename) {
	if (!loadPPM(filename)) {
		throw std::runtime_error("Failed to load PPM file: " + filename);
	}
}

bool Image::loadPPM(const std::string& filename) {
	std::cout << "Attempting to load texture: " << filename << std::endl;

	if (!std::filesystem::exists(filename)) {
		std::cerr << "File does not exist: " << filename << std::endl;
		return false;
	}

	std::ifstream file(filename, std::ios::binary);
	if (!file) {
		std::cout << "Failed to open file" << std::endl;
		return false;
	}

	std::string header;
	file >> header;
	if (header != "P6") {
		std::cout << "Header not P6" << std::endl;
		return false;
	}

	file >> width >> height;
	int maxValue;
	file >> maxValue;
	file.ignore(1);  // Skip the single whitespace after the header

	pixels.resize(width * height);

	for (int i = 0; i < width * height; ++i) {
		char r, g, b;
		file.get(r).get(g).get(b);
		pixels[i] = Color(
				static_cast<uint8_t>(r) / 255.0f,
				static_cast<uint8_t>(g) / 255.0f,
				static_cast<uint8_t>(b) / 255.0f
		);
	}

	return true;
}

Color Image::getPixelColor(int x, int y) const {
	if (x < 0 || x >= width || y < 0 || y >= height) {
		throw std::out_of_range("Pixel coordinates out of bounds");
	}
	return pixels[y * width + x];
}

