#ifndef RAYTRACER_LIGHT_H
#define RAYTRACER_LIGHT_H
#include "Vector3.h"
#include "Color.h"

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

/*Light
Attributes:
std::string type; // e.g., "pointlight"
Vector3 position;
Vector3 intensity;
Methods:
Light-specific methods as needed.*/
};






#endif //RAYTRACER_LIGHT_H
