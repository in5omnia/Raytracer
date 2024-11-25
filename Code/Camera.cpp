#include "Camera.h"


/* Camera */

Camera::Camera(int type, int width, int height,
							 Vector3 position, Vector3 lookAt, Vector3 upVector,
							 float fov, float exposure,
							 float apertureRadius, float focalDistance) :
							 type(type), width(width), height(height),
							 position(position), lookAt(lookAt), upVector(upVector),
							 fov(fov), exposure(exposure) {
	if (type == LENS) {
		this->apertureRadius = apertureRadius;
		this->focalDistance = focalDistance;
	} else {
		this->apertureRadius = 0.0f;
		this->focalDistance = 0.0f;
	}
	//compute camera's basis vectors
	calculateForwardVector();
	calculateRightVector();
	setUpVector(crossProduct(rightVector, forwardVector));	//TODO: check if it makes a difference
}


//methods
/**
 * Generates a ray for given normalized screen coordinates.
*/
Ray Camera::generateRayPinhole(float u, float v) const {

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

Ray Camera::generateRayLens(Ray pinholeRay) const {
	// Calculate the point on the focal plane
	Vector3 focalPoint = pinholeRay.getOrigin() + pinholeRay.getDirection() * focalDistance;

	// Sample a random point on the aperture disk
	float r = apertureRadius * sqrt(static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
	float theta = 2.0f * M_PI * static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	Vector3 apertureSamplePoint = position + rightVector * (r * cos(theta)) + upVector * (r * sin(theta));

	// Create a new ray from the aperture sample to the focal point
	Vector3 apertureSampleDir = (focalPoint - apertureSamplePoint).normalize();
	return Ray(apertureSamplePoint, apertureSampleDir);
}



//getters
int Camera::getWidth() const { return width; }
int Camera::getHeight() const { return height; }
Vector3 Camera::getPosition() const { return position; }
Vector3 Camera::getLookAt() const { return lookAt; }
Vector3 Camera::getUpVector() const { return upVector; }
float Camera::getFov() const { return fov; }
int Camera::getType() const { return type; }

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
 void Camera::setFov(float _fov) { fov = _fov; }

//methods
void Camera::calculateForwardVector() {
	this->forwardVector = (lookAt - position).normalize();
}

void Camera::calculateRightVector() {	// must be called after calculateForwardVector!!
	this->rightVector = crossProduct(forwardVector, upVector).normalize();
}



