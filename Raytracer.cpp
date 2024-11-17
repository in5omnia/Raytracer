#include "Raytracer.h"

Raytracer::Raytracer(int nbounces,
		  std::string rendermode,
		  PinholeCamera& camera,
		  Scene& scene
) :  nbounces(nbounces), rendermode(rendermode), camera(camera), scene(scene) {}


void Raytracer::render(Image& image) {
	int width = image.getWidth();
	int height = image.getHeight();

	for (int y = 0; y < height; ++y) {		//bottom to top
		for (int x = 0; x < width; ++x) {	//left to right
			//Normalized pixel coordinates
			float u = 1.0f - (static_cast<float>(x) + 0.5f) / static_cast<float>(width);	//(subtracting from 1 because before it was flipped)
			float v = (static_cast<float>(y) + 0.5f) / static_cast<float>(height);
			v = 1.0f - v; // Flip v if necessary

			Ray ray = camera.generateRay(u, v);
			Color color = traceRay(ray);

			image.setPixelColor(x, y, color);
		}
	}
}

Color Raytracer::traceRay(const Ray& ray) {
	// TODO: Implement ray tracing logic here
	//  + consider the number of bounces

	if (rendermode == "binary"){
		float t;  // Distance to the closest intersection
		if (scene.intersect(ray, t)) {
			// If an intersection occurs, return red for the object
			std::cout << "Raytracer: Intersection detected" << std::endl;
			return Color(1.0f, 0.0f, 0.0f);  // Red color
		}

		// No intersection: return black for background
		return Color(0.0f, 0.0f, 0.0f);  // Black color
	}
	else if (rendermode == "phong") {
		float t;  // Distance to the closest intersection
		if (scene.intersect(ray, t)) {
			return shadeBlinnPhong(ray, t);  // Blinn-Phong color
		}
		// No intersection
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


Color Raytracer::shadeBlinnPhong(const Ray& ray, float& t) {
	std::shared_ptr<Shape> hitObject = scene.getLastHitObject();  // Get the intersected object
	Material material = hitObject->getMaterial();

	Vector3 intersectionPoint = ray.pointAtParameter(t);
	Vector3 n_normal = hitObject->getNormal(intersectionPoint);  // Normal at intersection
	Vector3 v_viewDir = (ray.getOrigin() - intersectionPoint).normalize();

	Color finalColor(0.0f, 0.0f, 0.0f);  // Initialize final color
	/*Color ambientColor = material.diffuseColor * 0.1f;  // Adjust base ambient light

    finalColor = ambientColor;  // Start with ambient contribution*/

	for (const std::shared_ptr<Light>& light : scene.getLights()) {
		Vector3 l_lightDir = (light->getPosition() - intersectionPoint).normalize();
		Color lightIntensity = light->getIntensity();

		/*//Check for shadows (optional but recommended)
		if (scene.isInShadow(intersectionPoint, lightDir)) {
			continue;  // Skip light contribution if in shadow
		}*/

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

	return finalColor.clamp(0.0f, 1.0f);  // Clamp color values
}


void Raytracer::readJSON(const std::string& filename) {
	std::ifstream file(filename);
	if (!file) {
		throw std::runtime_error("Could not open JSON file: " + filename);
	}

	nlohmann::json j = nlohmann::json::parse(false, file);

	// Load raytracer settings
	nbounces = j["nbounces"];
	rendermode = j["rendermode"];

	// Load camera
	auto camData = j["camera"];
	camera = PinholeCamera(
			camData["width"],
			camData["height"],
			Vector3(camData["position"][0], camData["position"][1], camData["position"][2]),
			Vector3(camData["lookAt"][0], camData["lookAt"][1], camData["lookAt"][2]),
			Vector3(camData["upVector"][0], camData["upVector"][1], camData["upVector"][2]),
			camData["fov"],
			camData["exposure"]
	);

	// Load scene
	auto sceneData = j["scene"];
	scene.setBackgroundColor(Color(
			sceneData["backgroundcolor"][0],
			sceneData["backgroundcolor"][1],
			sceneData["backgroundcolor"][2]
	));

	// Load lights
	for (const auto& lightData : sceneData["lightsources"]) {
		if (lightData["type"] == "pointlight") {
			scene.addLight(std::make_shared<PointLight>(
					Vector3(lightData["position"][0], lightData["position"][1], lightData["position"][2]),
					Color(lightData["intensity"][0], lightData["intensity"][1], lightData["intensity"][2])
			));
		}
	}

	// Load shapes
	for (const auto& shapeData : sceneData["shapes"]) {
		auto materialData = shapeData["material"];
		Material material(
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
}

