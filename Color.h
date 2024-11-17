//
// Created by Beatriz Gavilan on 16/11/2024.
//

#ifndef RAYTRACER_COLOR_H
#define RAYTRACER_COLOR_H
#include <cstdint>

class Color {
	private:
		uint8_t r, g, b;
	public:
	// Default constructor
		Color();
		/*Color(uint8_t r, uint8_t g, uint8_t b);*/
		Color(float r, float g, float b);
		~Color();
		//getters
		uint8_t getR() const;
		uint8_t getG() const;
		uint8_t getB() const;
		//setters
		void setR(unsigned int r);
		void setG(unsigned int g);
		void setB(unsigned int b);
		void setRGB(unsigned int r, unsigned int g, unsigned int b);
		void setRGBFloat(float r, float g, float b);

		/*void scaleRGB(float factor);
		void addRGB(const Color& color);
		void addRGBFloat(float r, float g, float b);
		void subtractRGB(const Color& color);
		void subtractRGBFloat(float r, float g, float b);
		void multiplyRGB(const Color& color);
		void multiplyRGBFloat(float r, float g, float b);
		void divideRGB(const Color& color);
		void divideRGBFloat(float r, float g, float b);
		void clampRGB();
		void gammaCorrect(float gamma);
		void gammaCorrectRGB(float gamma);
		void gammaCorrectRGBFloat(float gamma);
		void toFloat(float* r, float* g, float* b);
		void toFloat(float* rgb);
		void toInt(unsigned int* r, unsigned int* g, unsigned int* b);
		void toInt(unsigned int* rgb);
		void toPPM(std::ostream& out);
		void toPPM(std::ostream& out, int max);
		void toPPM(std::ostream& out, int max, int indent);
		void toPPM(std::ostream& out, int max, int indent, int width);
		void toPPM(std::ostream& out, int max, int indent, int width, int precision);
		void toPPM(std::ostream& out, int max, int indent, int width, int precision, bool newline);
		void toPPM(std::ostream& out, int max, int indent, int width, int precision, bool newline, bool flush);
		void toPPM(std::ostream& out, int max, int indent, int width, int precision, bool newline, bool flush, bool hex);
		void toPPM(std::ostream& out, int max, int indent, int width, int precision, bool newline, bool flush, bool hex, bool upper);
		void toPPM(std::ostream& out, int max, int indent, int width,*/
};


#endif //RAYTRACER_COLOR_H
