#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <vector>
#include <string>
#include <cstdint>
#include "Color.h"

class Image {
	private:
		std::vector<Color> pixels;	//Before: std::vector<uint8_t> pixels;
		int width;	//same as camera?
		int height;

	public:
		Image(int w, int h);

		// Integer version (0-255)
		void setPixelColor(int x, int y, uint8_t r, uint8_t g, uint8_t b);

		void setPixelColor(int x, int y, Color color);

		// Float version (0.0-1.0)
		void setPixelColorFloat(int x, int y, float r, float g, float b);

		bool writePPM(const std::string& filename) const;
		int getWidth() const;
		int getHeight() const;
};

#endif // IMAGE_HPP