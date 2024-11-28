#include "Color.h"

// Default constructor
Color::Color() : r(0.0f), g(0.0f), b(0.0f) {}
/*Color::Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}*/
//Color::Color(float r, float g, float b) : r(static_cast<uint8_t>(r)), g(static_cast<uint8_t>(g)), b(static_cast<uint8_t>(b)) {}
Color::Color(float r, float g, float b) : r(r), g(g), b(b) {}
Color::~Color() {}

float Color::getR() const {	return r;	}
float Color::getG() const {	return g;	}
float Color::getB() const {	return b;	}

void Color::setR(float r) {	this->r = r;	}
void Color::setG(float g) {	this->g = g;	}
void Color::setB(float b) {	this->b = b;	}
void Color::setRGB(float r, float g, float b) {
	this->r = r;
	this->g = g;
	this->b = b;
}
/*void Color::setRGBFloat(float r, float g, float b) {
	this->r = static_cast<unsigned int>(r);
	this->g = static_cast<unsigned int>(g);
	this->b = static_cast<unsigned int>(b);
}*/

Color Color::operator*(float s) const{
	return Color(r * s, g * s, b * s);
}

Color Color::operator/(float s) const {
	return Color(r / s, g / s, b / s);
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


std::string Color::toString() const {
	return "( " + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + " )";
}


bool Color::operator==(const Color &color) const {
	return r == color.r && g == color.g && b == color.b;
}


Color Color::linearToneMap(float maxIntensity) const {
	float scale = 1.0f / maxIntensity;  // Scale to normalize colors
	/*return Color(
			std::min(r * scale, 1.0f),  // Clamp each channel to [0, 1]
			std::min(g * scale, 1.0f),
			std::min(b * scale, 1.0f)
	);*/
	return Color(r * scale, g * scale, b * scale);
}


// isZero function
bool Color::isZero() const {
	const float epsilon = 1e-6f;  // Small threshold to handle floating-point precision
	return (fabs(r) < epsilon) && (fabs(g) < epsilon) && (fabs(b) < epsilon);
}

// Gamma correction function in Color class
Color Color::gammaCorrect(float gamma) const {
	return Color(std::pow(r, gamma), std::pow(g, gamma), std::pow(b, gamma));
}