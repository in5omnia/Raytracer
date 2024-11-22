#include "Raytracer.h"
#include <omp.h>

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

			Ray ray = camera->generateRay(u, v);
			Color color = traceRay(ray);
			image.setPixelColor(x, y, color);
		}
	}
}

Color Raytracer::traceRay(const Ray& ray) {
	// TODO: Implement ray tracing logic here
	//  + consider the number of bounces

	float t;  // Distance to the closest intersection
	std::shared_ptr<Shape> hitObject = scene.intersect(ray, t, false, 0.0f, nullptr);
	// Intersection detected
	if (hitObject != nullptr) {
		if (rendermode == "binary"){
			return Color(1.0f, 0.0f, 0.0f);  // Red color
		}
		else if (rendermode == "phong") {
			return shadeBlinnPhong(ray, t, hitObject);  // Blinn-Phong color
		}
	}
	// No intersection
	if (rendermode == "binary"){
		// No intersection: return black for background
		return Color(0.0f, 0.0f, 0.0f);  // Black color
	}
	else if (rendermode == "phong") {
		// No intersection: return background color
		//std::cout << "background" << "[" << scene.getBackgroundColor().getR() << scene.getBackgroundColor().getG() << scene.getBackgroundColor().getB() << "]" << std::endl;
		return scene.getBackgroundColor();
	}




	if (nbounces == 0) {std::cout << "REMOVE ME" << std::endl;} //TODO: remove this line (compiler fix)
	return Color();	//TODO: remove this line
	/*
	Should handle:
	- Intersection tests with all objects in the scene. -> Scene::intersect()
	- Shading calculations using the Blinn-Phong model.
	- Handling reflections and refractions if implemented.
	- Background color if no intersection occurs.
	*/
}


Color Raytracer::shadeBlinnPhong(const Ray& ray, float& t, std::shared_ptr<Shape> hitObject) {
	//std::cout << "Blinn Phong" << std::endl;
	Material material;
	Vector3 intersectionPoint;
	Vector3 n_normal;  // Normal at intersection
	
	//#pragma omp critical
	//{
		material = hitObject->getMaterial();
		intersectionPoint = ray.pointAtParameter(t);
		n_normal = hitObject->getNormal(intersectionPoint);  // Normal at intersection
	//}
	Vector3 v_viewDir = (ray.getOrigin() - intersectionPoint).normalize();


	Color finalColor(0.0f, 0.0f, 0.0f);  // Initialize final color
	Color ambientColor = material.getDiffuseColor() * Ka;  // Adjust base ambient light
    finalColor += ambientColor;  // Start with ambient contribution

	for (const std::shared_ptr<Light>& light : scene.getLights()) {
		Vector3 l_lightDir = (light->getPosition() - intersectionPoint).normalize();
		Color lightIntensity = light->getIntensity();
		float lightDistance = (light->getPosition() - intersectionPoint).norm();

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
		camera = std::make_shared<PinholeCamera>(
				camData["width"],
				camData["height"],
				Vector3(camData["position"][0], camData["position"][1], camData["position"][2]),
				Vector3(camData["lookAt"][0], camData["lookAt"][1], camData["lookAt"][2]),
				Vector3(camData["upVector"][0], camData["upVector"][1], camData["upVector"][2]),
				camData["fov"],
				camData["exposure"]
		);
	} // if other types of cameras are added, add them here

	std::cout << "Camera loaded" << "width" << camera->getWidth() << std::endl;
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
				scene.addLight(std::make_shared<PointLight>(
						Vector3(lightData["position"][0], lightData["position"][1], lightData["position"][2]),
						Color(lightData["intensity"][0], lightData["intensity"][1], lightData["intensity"][2])
				));
			}
			std::cout << "Light" << scene.getLights()[0]->getPosition() << std::endl;
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
			scene.addShape(std::make_shared<Sphere>(
					Vector3(shapeData["center"][0], shapeData["center"][1], shapeData["center"][2]),
					shapeData["radius"],
					material
			));
		} else if (shapeData["type"] == "cylinder") {
			scene.addShape(std::make_shared<Cylinder>(
					Vector3(shapeData["center"][0], shapeData["center"][1], shapeData["center"][2]),
					Vector3(shapeData["axis"][0], shapeData["axis"][1], shapeData["axis"][2]),
					shapeData["radius"],
					shapeData["height"],
					material
			));
		} else if (shapeData["type"] == "triangle") {
			scene.addShape(std::make_shared<Triangle>(
					Vector3(shapeData["v0"][0], shapeData["v0"][1], shapeData["v0"][2]),
					Vector3(shapeData["v1"][0], shapeData["v1"][1], shapeData["v1"][2]),
					Vector3(shapeData["v2"][0], shapeData["v2"][1], shapeData["v2"][2]),
					material
			));
		}
	}
	return Image(camera->getWidth(), camera->getHeight());
}

