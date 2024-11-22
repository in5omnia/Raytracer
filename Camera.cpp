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


float PinholeCamera::getFov() const { return fov; }
void PinholeCamera::setFov(float _fov) { fov = _fov; }

/* Orthographic Camera */
OrthographicCamera::OrthographicCamera(int width, int height,
									   Vector3 position,
									   Vector3 lookAt,
									   Vector3 upVector,
									   float exposure) {
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


Ray OrthographicCamera::generateRay(float u, float v) const {

	// Compute the ray's origin on the image plane (0.5f is subtracted to center the ray)
	Vector3 rayOrigin = position + rightVector * (u - 0.5f) * width + upVector * (v - 0.5f) * height;

	// The ray direction is fixed for orthographic projection
	Vector3 rayDirection = forwardVector;  // Typically normalized during basis vector computation

	// Return the ray
	return Ray(rayOrigin, rayDirection);
}



/* Camera */

//getters
int Camera::getWidth() const { return width; }
int Camera::getHeight() const { return height; }
Vector3 Camera::getPosition() const { return position; }
Vector3 Camera::getLookAt() const { return lookAt; }
Vector3 Camera::getUpVector() const { return upVector; }

float Camera::getExposure() const { return exposure; }
Vector3 Camera::getForwardVector() const { return forwardVector; }
Vector3 Camera::getRightVector() const { return rightVector; }

//setters
 void Camera::setWidth(int _width) { width = _width; }
 void Camera::setHeight(int _height) { height = _height; }
 void Camera::setPosition(Vector3 _position) { position = _position; }
 void Camera::setLookAt(Vector3 _lookAt) { lookAt = _lookAt; }
 void Camera::setUpVector(Vector3 _upVector) { upVector = _upVector; }
 void Camera::setExposure(float _exposure) { exposure = _exposure; }

//methods
void Camera::calculateForwardVector() {
	this->forwardVector = (lookAt - position).normalize();
}

void Camera::calculateRightVector() {	// must be called after calculateForwardVector!!
	this->rightVector = crossProduct(forwardVector, upVector).normalize();
}



