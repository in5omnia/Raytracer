#include "Camera.h"


/* PinholeCamera */

PinholeCamera::PinholeCamera(int width, int height,
			   Vector3 position,
			   Vector3 lookAt,
			   Vector3 upVector,
			   float fov, float exposure) : fov(fov){
	this->width = width;
	this->height = height;
	this->position = position;
	this->lookAt = lookAt;
	this->upVector = upVector;
	this->exposure = exposure;

	//compute camera's basis vectors
	calculateForwardVector();
	calculateRightVector();
	setUpVector(crossProduct(rightVector, forwardVector));	//TODO: check if it makes a difference
}


//methods
/**
 * Generates a ray for given normalized screen coordinates.
*/
Ray PinholeCamera::generateRay(float u, float v) const {

	// Compute image plane dimensions
	float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	float fovRadians = fov * (M_PI / 180.0f);	//convert to radians
	float viewportHeight = 2.0f * tanf(fovRadians / 2.0f);	//view plane height
	float viewportWidth = aspectRatio * viewportHeight; 	//view plane width

	// Compute horizontal and vertical spans
	Vector3 horizontal = rightVector * viewportWidth;
	Vector3 vertical = upVector * viewportHeight;

	// Compute lower-left corner of the image plane
	Vector3 lowerLeftCorner = position + forwardVector - (horizontal * 0.5f) - (vertical * 0.5f);

	// Compute the ray direction
	Vector3 rayDirection = (lowerLeftCorner + horizontal * u + vertical * v) - position;
	rayDirection = rayDirection.normalize();

	// Return the ray starting from the camera position
	return Ray(position, rayDirection);
}



//getters
int PinholeCamera::getWidth() const { return width; }
int PinholeCamera::getHeight() const { return height; }
Vector3 PinholeCamera::getPosition() const { return position; }
Vector3 PinholeCamera::getLookAt() const { return lookAt; }
Vector3 PinholeCamera::getUpVector() const { return upVector; }
float PinholeCamera::getFov() const { return fov; }

float PinholeCamera::getExposure() const { return exposure; }
Vector3 PinholeCamera::getForwardVector() const { return forwardVector; }
Vector3 PinholeCamera::getRightVector() const { return rightVector; }



//setters
 void PinholeCamera::setWidth(int _width) { width = _width; }
 void PinholeCamera::setHeight(int _height) { height = _height; }
 void PinholeCamera::setPosition(Vector3 _position) { position = _position; }
 void PinholeCamera::setLookAt(Vector3 _lookAt) { lookAt = _lookAt; }
 void PinholeCamera::setUpVector(Vector3 _upVector) { upVector = _upVector; }
 void PinholeCamera::setExposure(float _exposure) { exposure = _exposure; }
 void PinholeCamera::setFov(float _fov) { fov = _fov; }

//methods
void PinholeCamera::calculateForwardVector() {
	this->forwardVector = (lookAt - position).normalize();
}

void PinholeCamera::calculateRightVector() {	// must be called after calculateForwardVector!!
	this->rightVector = crossProduct(forwardVector, upVector).normalize();
}



