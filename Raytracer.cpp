#include "Raytracer.h"
#include <omp.h>
#include <stack>  // Include this at the top of your file

Raytracer::Raytracer() {}


void Raytracer::render(Image& image) {
	int width = image.getWidth();
	int height = image.getHeight();

	//#pragma omp parallel for
	for (int y = 0; y < height; ++y) {		//bottom to top
		for (int x = 0; x < width; ++x) {	//left to right
			//Normalized pixel coordinates
			float u = 1.0f - (static_cast<float>(x) + 0.5f) / static_cast<float>(width);	//(subtracting from 1 because before it was flipped)
			float v = (static_cast<float>(y) + 0.5f) / static_cast<float>(height);
			v = 1.0f - v; // Flip v if necessary

			Ray ray = camera.generateRay(u, v);
			std::stack<float> refractiveStack;
			Color color = traceRay(ray, 0, refractiveStack);
			// Apply linear tone mapping
			color = color * camera.getExposure(); //not if exposure is too low
			float maxIntensity = std::max(color.getR(), std::max(color.getG(), color.getB()));
			if (maxIntensity > 1.0f) {
				color = color.linearToneMap(maxIntensity);
			}
			image.setPixelColor(x, y, color);
		}
	}
}



Color Raytracer::traceRay(const Ray& ray, int depth, std::stack<float> refractiveStack) {
	// Base case: Limit the number of bounces
	if (depth > nbounces) {
		return Color(0.0f, 0.0f, 0.0f);  // Black color for exceeded recursion
	}

	float t;  // Distance to the closest intersection
	Shape hitObject = scene.intersect(ray, t, false, 0.0f, Shape(NO_SHAPE));
	Color localColor;

	// Intersection detected
	if (hitObject.getShapeType() != NO_SHAPE) {
		if (rendermode == "binary") {
			return Color(1.0f, 0.0f, 0.0f);  // Red color
		} else if (rendermode == "phong") {
			// Local shading using Blinn-Phong
			localColor = shadeBlinnPhong(ray, t, hitObject);

			// Retrieve material and intersection details
			Material material = hitObject.getMaterial();
			Vector3 intersectionPoint = ray.pointAtParameter(t);
			Vector3 normal = hitObject.getNormal(intersectionPoint);

			// **Refraction Logic**: Only process if the material is refractive
			Color refractionColor(0.0f, 0.0f, 0.0f);  // Initialize refraction contribution
			if (material.getIsRefractive() && depth < nbounces) {
				Vector3 viewDir = ray.getDirection().normalize() * -1.0f;  // View direction
				bool entering = dotProduct(viewDir, normal) > 0;  // Determine if entering or exiting
				Vector3 adjustedNormal = entering ? normal : normal * -1.0f;

				// Compute refractive indices
				float n1 = refractiveStack.empty() ? 1.0f : refractiveStack.top();
				float n2 = entering ? material.getRefractiveIndex() : (refractiveStack.size() > 1 ? refractiveStack.top() : 1.0f);
				/*float n1 = entering ? 1.0f : material.getRefractiveIndex();
				float n2 = entering ? material.getRefractiveIndex() : 1.0f;*/
				float eta = n1 / n2;

				// Compute the cosine of the incident angle
				float cosTheta1 = -dotProduct(ray.getDirection(), adjustedNormal);
				float sin2Theta2 = eta * eta * (1.0f - cosTheta1 * cosTheta1);

				if (sin2Theta2 <= 1.0f) {  // No total internal reflection
					float cosTheta2 = sqrt(1.0f - sin2Theta2);
					Vector3 refractDir = ray.getDirection() * eta + adjustedNormal * (eta * cosTheta1 - cosTheta2);
					refractDir = refractDir.normalize();

					// Update stack
					if (entering) {
						refractiveStack.push(material.getRefractiveIndex());
					} else if (!refractiveStack.empty()) {
						refractiveStack.pop();
						//refractiveStack.push(material.getRefractiveIndex());
					}

					// Offset slightly to avoid self-intersection
					Ray refractRay(intersectionPoint - adjustedNormal * 1e-4, refractDir);
					refractionColor = traceRay(refractRay, depth + 1, refractiveStack) * (1.0f - material.getReflectivity());
				}
				if (hitObject.toString() == "Cylinder") std::cout << "Depth " << depth << " n1: " << n1 << " n2: " << n2 << " Color " << refractionColor.toString() << std::endl;
			}

			// **Reflection Logic**: Keep existing reflection code intact
			if (material.getIsReflective() && depth < nbounces) {
				Vector3 reflectDir = ray.getDirection() - normal * 2.0f * dotProduct(ray.getDirection(), normal);
				reflectDir = reflectDir.normalize();

				Ray reflectRay(intersectionPoint + normal * 1e-4, reflectDir);  // Offset to avoid self-intersection
				Color reflectionColor = traceRay(reflectRay, depth + 1, refractiveStack);

				// Combine local, reflected, and refracted colors
				localColor = localColor * (1.0f - material.getReflectivity()) +
							 reflectionColor * material.getReflectivity() +
							 refractionColor;
			} else {
				// Combine local and refracted colors (if no reflection)
				localColor += refractionColor;
			}

			if (hitObject.toString() != "Cylinder") std::cout << "Depth " << depth << " hitting : " << hitObject.toString() << " Color " << localColor.toString() << std::endl;
			return localColor;
		}
	}

	// No intersection
	if (rendermode == "binary") {
		// No intersection: return black for background
		return Color(0.0f, 0.0f, 0.0f);  // Black color
	} else if (rendermode == "phong") {
		// No intersection: return background color
		return scene.getBackgroundColor();
	}
}


Color Raytracer::shadeBlinnPhong(const Ray& ray, float& t, Shape hitObject) {
	//std::cout << "Blinn Phong" << std::endl;
	Material material;
	Vector3 intersectionPoint;
	Vector3 n_normal;  // Normal at intersection
	
	//#pragma omp critical
	//{
		material = hitObject.getMaterial();
		intersectionPoint = ray.pointAtParameter(t);
		n_normal = hitObject.getNormal(intersectionPoint);  // Normal at intersection
	//}
	Vector3 v_viewDir = (ray.getOrigin() - intersectionPoint).normalize();


	Color finalColor(0.0f, 0.0f, 0.0f);  // Initialize final color
	Color ambientColor = material.getDiffuseColor() * Ka;  // Adjust base ambient light
    finalColor += ambientColor;  // Start with ambient contribution

	for (PointLight light : scene.getPointLights()) {
		Vector3 l_lightDir = (light.getPosition() - intersectionPoint).normalize();
		Color lightIntensity = light.getIntensity();
		float lightDistance = (light.getPosition() - intersectionPoint).norm();

		//Check for shadows
		if (scene.isInShadow(intersectionPoint, l_lightDir, lightDistance, n_normal, hitObject)) {
			continue;  // Skip light contribution if in shadow
		}

		// Diffuse component
		float diffuseFactor = std::max(0.0f, dotProduct(n_normal, l_lightDir));
		Color diffuse = material.getDiffuseColor() * material.getKd() * lightIntensity * diffuseFactor;

		// Specular component (Blinn-Phong)
		Vector3 h_halfwayDir = (l_lightDir + v_viewDir).normalize();
		float specFactor = std::pow(std::max(0.0f, dotProduct(n_normal, h_halfwayDir)), material.getSpecularExponent());
		Color specular = material.getSpecularColor() * material.getKs() * lightIntensity * specFactor;

		// Combine components for this light
		finalColor += diffuse + specular;
	}

	//finalColor = finalColor * 0.9f;
	return finalColor.clamp(0.0f, 1.0f);  // Clamp color values
}


Image Raytracer::readJSON(const std::string& filename) {
	std::ifstream file(filename);
	if (!file) {
		throw std::runtime_error("Could not open JSON file: " + filename);
	}

	nlohmann::json j = nlohmann::json::parse(file);

	// Load raytracer settings
	if (j.contains("nbounces")) {
		nbounces = j["nbounces"];
	} else {
		nbounces = 1;
	}
	rendermode = j["rendermode"];

	// Load camera
	auto camData = j["camera"];
	if (camData["type"] == "pinhole") {
		camera = PinholeCamera(
				camData["width"],
				camData["height"],
				Vector3(camData["position"][0], camData["position"][1], camData["position"][2]),
				Vector3(camData["lookAt"][0], camData["lookAt"][1], camData["lookAt"][2]),
				Vector3(camData["upVector"][0], camData["upVector"][1], camData["upVector"][2]),
				camData["fov"],
				camData["exposure"]
		);
	} // if other types of cameras are added, add them here

	std::cout << "Camera loaded" << "width" << camera.getWidth() << std::endl;
	// Load scene
	auto sceneData = j["scene"];
	Color backgroundColor(
			sceneData["backgroundcolor"][0],
			sceneData["backgroundcolor"][1],
			sceneData["backgroundcolor"][2]
	);
	scene.setBackgroundColor(backgroundColor);

	if (sceneData.contains("lightsources")) {
		// Load lights
		for (const auto& lightData : sceneData["lightsources"]) {
			if (lightData["type"] == "pointlight") {
				scene.addPointLight(PointLight(
						Vector3(lightData["position"][0], lightData["position"][1], lightData["position"][2]),
						Color(lightData["intensity"][0], lightData["intensity"][1], lightData["intensity"][2])
				));
			}
			std::cout << "Light" << scene.getPointLights()[0].getPosition() << std::endl;
		}
	}
	std::cout << "Lights loaded" << std::endl;
	// Load shapes
	for (const auto& shapeData : sceneData["shapes"]) {
		Material material;
		if (shapeData.contains("material")) {
			auto materialData = shapeData["material"];
			material = Material(
					materialData["ks"],
					materialData["kd"],
					materialData["specularexponent"],
					Color(materialData["diffusecolor"][0], materialData["diffusecolor"][1], materialData["diffusecolor"][2]),
					Color(materialData["specularcolor"][0], materialData["specularcolor"][1], materialData["specularcolor"][2]),
					materialData["isreflective"],
					materialData["reflectivity"],
					materialData["isrefractive"],
					materialData["refractiveindex"]
			);
		} else {
			material = Material(0.5f, 0.5f, 32, Color(1, 1, 1), Color(1, 1, 1), false, 0.0f, false, 1.0f);
		}

		if (shapeData["type"] == "sphere") {
			scene.addSphere(Sphere(
					Vector3(shapeData["center"][0], shapeData["center"][1], shapeData["center"][2]),
					shapeData["radius"],
					material
			));
		} else if (shapeData["type"] == "cylinder") {
			scene.addCylinder(Cylinder(
					Vector3(shapeData["center"][0], shapeData["center"][1], shapeData["center"][2]),
					Vector3(shapeData["axis"][0], shapeData["axis"][1], shapeData["axis"][2]),
					shapeData["radius"],
					shapeData["height"],
					material
			));
		} else if (shapeData["type"] == "triangle") {
			scene.addTriangle(Triangle(
					Vector3(shapeData["v0"][0], shapeData["v0"][1], shapeData["v0"][2]),
					Vector3(shapeData["v1"][0], shapeData["v1"][1], shapeData["v1"][2]),
					Vector3(shapeData["v2"][0], shapeData["v2"][1], shapeData["v2"][2]),
					material
			));
		}
	}
	return Image(camera.getWidth(), camera.getHeight());
}

