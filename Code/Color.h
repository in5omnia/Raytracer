//
// Created by Beatriz Gavilan on 16/11/2024.
//

#ifndef RAYTRACER_COLOR_H
#define RAYTRACER_COLOR_H
#include <cstdint>
#include <string>

class Color {
	private:
		float r, g, b;
	public:
		// Default constructor
		Color();
		Color(float r, float g, float b);
		~Color();

		//getters
		float getR() const;
		float getG() const;
		float getB() const;

		//setters
		void setR(float r);
		void setG(float g);
		void setB(float b);
		void setRGB(float r, float g, float b);

		//operators
		Color operator*(float s) const;
		Color operator/(float s) const;
		Color operator+(const Color& color) const;
		Color operator-(const Color& color) const;
		Color operator+=(const Color& color);
		//element-wise multiplication
		Color operator*(const Color& color) const;
		bool operator==(const Color& color) const;

		Color gammaCorrect(float gamma) const;
		Color clamp(float min, float max) const ;
		Color linearToneMap(float maxIntensity) const;

		bool isZero() const;
		std::string toString() const;
};


#endif //RAYTRACER_COLOR_H
