#ifndef RAYTRACER_LIGHT_H
#define RAYTRACER_LIGHT_H
#include "Vector3.h"
#include "Color.h"
#include <random>


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

};






#endif //RAYTRACER_LIGHT_H
