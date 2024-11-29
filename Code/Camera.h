#ifndef RAYTRACER_CAMERA_H
#define RAYTRACER_CAMERA_H

#include <iostream>
#include <string>
#include "Vector3.h"
#include "Ray.h"

#define PINHOLE 0
#define LENS 1

class Camera {
	private:
		int type;
		int width;	//image res
		int height; //image res
		Vector3 position;
		Vector3 lookAt;
		Vector3 upVector;
		float exposure;
		float fov;

		//lens specific
		float apertureRadius;
		float focalDistance;

		Vector3 forwardVector;
		Vector3 rightVector;

		void calculateForwardVector();
		void calculateRightVector();

	public:
		//constructor
		Camera()= default;
		Camera(int type, int width, int height,
			   Vector3 position,
			   Vector3 lookAt,
			   Vector3 upVector,
			   float fov, float exposure,
			   float apertureRadius, float focalDistance);

		~Camera() = default; // destructor

		//methods
		Ray generateRayPinhole(float x, float y) const;
		Ray generateRayLens(Ray pinholeRay) const;

		//getters
		int getType() const;
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