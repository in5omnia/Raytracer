#ifndef RAYTRACER_LIGHT_H
#define RAYTRACER_LIGHT_H
#include "Vector3.h"
#include "Color.h"

class Light {
	protected:
		Vector3 position;
		Color intensity;
	public:
		Light();
		~Light();
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

class PointLight : public Light {
	public:
		PointLight(Vector3 position, Color intensity);
		~PointLight();
};




#endif //RAYTRACER_LIGHT_H
