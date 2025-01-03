#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H
#include "Vector3.h"

class Ray {
	private:
		Vector3 origin;
		Vector3 direction;
	public:
		Ray(Vector3 origin, Vector3 direction);
		~Ray(); // destructor

		Vector3 pointAtParameter(float t) const; // Returns a point along the ray.

		//getters
		Vector3 getOrigin() const;
		Vector3 getDirection() const;
};


#endif //RAYTRACER_RAY_H
