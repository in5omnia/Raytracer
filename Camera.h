#ifndef RAYTRACER_CAMERA_H
#define RAYTRACER_CAMERA_H

#include <iostream>
#include <string>
#include "Vector3.h"
#include "Ray.h"


class PinholeCamera {
	private:
		int width;	//image res //TODO: int or float?
		int height; //image res
		Vector3 position;
		Vector3 lookAt;
		Vector3 upVector;
		float exposure;
		float fov;

		Vector3 forwardVector;
		Vector3 rightVector;

		void calculateForwardVector();
		void calculateRightVector();

	public:
		//constructor
		PinholeCamera()= default;
		PinholeCamera(int width, int height,
			   Vector3 position,
			   Vector3 lookAt,
			   Vector3 upVector,
			   float fov, float exposure);

		~PinholeCamera() = default; // destructor

		//methods
		Ray generateRay(float x, float y) const;

		//getters
		std::string getType() const;
		int getWidth() const;
		int getHeight() const;
		Vector3 getPosition() const;
		Vector3 getLookAt() const;
		Vector3 getUpVector() const;
		float getExposure() const;
		Vector3 getForwardVector() const;
		Vector3 getRightVector() const;
		float getFov() const;

		//setters
		void setType(std::string _type);
		void setWidth(int _width);
		void setHeight(int _height);
		void setPosition(Vector3 _position);
		void setLookAt(Vector3 _lookAt);
		void setUpVector(Vector3 _upVector);
		void setExposure(float _exposure);
		void setFov(float _fov);

};

#endif //RAYTRACER_CAMERA_H