#include "Color.h"

// Default constructor
Color::Color() : r(0.0f), g(0.0f), b(0.0f) {}
/*Color::Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}*/
Color::Color(float r, float g, float b) : r(static_cast<uint8_t>(r * 255.0f)), g(static_cast<uint8_t>(g * 255.0f)), b(static_cast<uint8_t>(b * 255.0f)) {}
Color::~Color() {}

uint8_t Color::getR() const {	return r;	}
uint8_t Color::getG() const {	return g;	}
uint8_t Color::getB() const {	return b;	}

void Color::setR(unsigned int r) {	this->r = r;	}
void Color::setG(unsigned int g) {	this->g = g;	}
void Color::setB(unsigned int b) {	this->b = b;	}
void Color::setRGB(unsigned int r, unsigned int g, unsigned int b) {
	this->r = r;
	this->g = g;
	this->b = b;
}
void Color::setRGBFloat(float r, float g, float b) {
	this->r = static_cast<unsigned int>(r * 255.0f);
	this->g = static_cast<unsigned int>(g * 255.0f);
	this->b = static_cast<unsigned int>(b * 255.0f);
}

Color Color::operator*(float s) const{
	return Color(r * s, g * s, b * s);
}

Color Color::operator+(const Color& color) const{
	return Color(r + color.r, g + color.g, b + color.b);
}

Color Color::operator-(const Color& color) const {
	return Color(r - color.r, g - color.g, b - color.b);
}

Color Color::operator+=(const Color& color) {
	r += color.r;
	g += color.g;
	b += color.b;
	return *this;
}

Color Color::operator*(const Color& color) const {
	return Color(r * color.r, g * color.g, b * color.b);
}

/* auxiliary function for Color::clamp */
float clampFloat(float value, float min, float max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

Color Color::clamp(float min, float max) const {
	return Color(
			clampFloat(r, min, max),  // Clamp red
			clampFloat(g, min, max),  // Clamp green
			clampFloat(b, min, max)   // Clamp blue
	);
}



