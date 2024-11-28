#ifndef RAYTRACER_LIGHT_H
#define RAYTRACER_LIGHT_H
#include "Vector3.h"
#include "Color.h"
#include <random>


#define POINT 0
#define AREA 1
/*
class Light {
public:
	private:
		int type;
		Vector3 position;
		Color intensity;
		// Other properties...
	public:
		Light();
		Light(int type, Vector3 position, Color intensity);
		~Light();
		Vector3 getPosition() const;
		void sampleLight(const Vector3& referencePoint, std::default_random_engine& rng,
						 Vector3& lightPosition, Vector3& lightNormal, float& pdf) const {
			if (type == POINT) {
				// For point lights
				lightPosition = position;
				lightNormal = - (lightPosition - referencePoint).normalize();
				pdf = 1.0f;  // Since it's a single point
			} else if (type == AREA) {
				// For area lights, sample a point on the surface
				// Implement area light sampling here
				// Update lightPosition, lightNormal, and pdf accordingly
			}
		}

		Color getIntensity(const Vector3& intersectionPoint, const Vector3& lightPosition, const Vector3& lightNormal) const {
			if (type == POINT) {
				return intensity;
			} else if (type == AREA) {
				// Return emitted radiance of the area light
				return intensity;  // Adjust based on area light properties
			}
			return Color(0.0f, 0.0f, 0.0f);
		}

		// Other methods...
};*/


class PointLight {
	protected:
		Vector3 position;
		Color intensity;
	public:
		PointLight();
		PointLight(Vector3 position, Color intensity);
		~PointLight();
		Vector3 getPosition() const;
		Color getIntensity() const;
		void sampleLight(const Vector3& referencePoint, std::default_random_engine& rng,
						 Vector3& lightPosition, Vector3& lightNormal, float& pdf) const;

		Color getIntensity(const Vector3& intersectionPoint, const Vector3& lightPosition, const Vector3& lightNormal) const;

/*Light
Attributes:
std::string type; // e.g., "pointlight"
Vector3 position;
Vector3 intensity;
Methods:
Light-specific methods as needed.*/
};






#endif //RAYTRACER_LIGHT_H
