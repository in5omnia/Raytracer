#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <vector>
#include <string>
#include <cstdint>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "Color.h"

class Image {
	private:
		std::vector<Color> pixels;
		int width;
		int height;

	public:
		Image();
		Image(int w, int h);
		Image(const std::string& filename);  // New constructor to load PPM file

		void setPixelColor(int x, int y, Color color);

		bool writePPM(const std::string& filename) const;
		int getWidth() const;
		int getHeight() const;

		Color getPixelColor(int x, int y) const;  // Fetch color at (x, y)
		bool loadPPM(const std::string& filename);  // Load texture from PPM file
};

#endif // IMAGE_HPP