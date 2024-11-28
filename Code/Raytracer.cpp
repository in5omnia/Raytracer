#include "Raytracer.h"


Raytracer::Raytracer() {}


void Raytracer::render(Image& image) {
	if (camera.getType() == LENS) {
		if (rendermode == "brdf")
			return renderBRDFWithApertureSampling(image);
		return renderWithApertureSampling(image);
	} else if (rendermode == "brdf") {
		return renderBRDFWithoutApertureSampling(image);
	}
	int width = image.getWidth();
	int height = image.getHeight();

	scene.setBVHRoot(scene.buildBVH(scene.getShapes(), 0, scene.getShapes().size()));
	//scene.printTree(); //TODO:remove debug

	//#pragma omp parallel for collapse(2) schedule(static, 1)
	for (int y = 0; y < height; ++y) {		//bottom to top
		for (int x = 0; x < width; ++x) {	//left to right

			Color finalColor(0.0f, 0.0f, 0.0f);
			for (int s = 0; s < numPixelSamples; ++s) {  // Multi-sampling loop
				//Random offset for pixel sampling
				float offset_u = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
				float offset_v = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

				//Normalized pixel sample coordinates
				float u = (static_cast<float>(x) + offset_u) / static_cast<float>(width);
				float v = (static_cast<float>(y) + offset_v) / static_cast<float>(height);

				// Flip u and v
				u = 1.0f - u;
				v = 1.0f - v;

				Ray ray = camera.generateRayPinhole(u, v);
				std::stack<float> refractiveStack;
				finalColor += traceRay(ray, 0, refractiveStack);
			}

			// Average sample colors
			finalColor = finalColor / static_cast<float>(numPixelSamples);

			// Apply exposure
			float exposure = camera.getExposure();
			if (exposure < 0.5) exposure = 0.5f;	// Ensure exposure is not too low
			finalColor = finalColor * exposure;

			// Tone map to normalize intensities
			float maxIntensity = std::max(finalColor.getR(), std::max(finalColor.getG(), finalColor.getB()));
			if (maxIntensity > 1.0f) {
				finalColor = finalColor.linearToneMap(maxIntensity);
			}

			// Clamp final color to valid range
			finalColor.clamp(0.0f, 1.0f);

			// Save pixel color
			image.setPixelColor(x, y, finalColor);
		}
	}
}


void Raytracer::renderWithApertureSampling(Image& image) {
	int width = image.getWidth();
	int height = image.getHeight();

	scene.setBVHRoot(scene.buildBVH(scene.getShapes(), 0, scene.getShapes().size()));
	//scene.printTree(); //TODO:remove debug

	//#pragma omp parallel for collapse(2) schedule(static, 1)
	for (int y = 0; y < height; ++y) {		//bottom to top
		for (int x = 0; x < width; ++x) {	//left to right

			Color finalColor(0.0f, 0.0f, 0.0f);

			// ** Pixel Sampling **
			for (int s = 0; s < numPixelSamples; ++s) {
				//Random offset for pixel sampling
				float offset_u = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
				float offset_v = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

				//Normalized pixel sample coordinates
				float u = (static_cast<float>(x) + offset_u) / static_cast<float>(width);
				float v = (static_cast<float>(y) + offset_v) / static_cast<float>(height);

				// Flip u and v
				u = 1.0f - u;
				v = 1.0f - v;

				// ** Aperture Sampling (depth of field) **
				Color pixelSampleColor(0.0f, 0.0f, 0.0f);

				// Compute the pinhole ray as usual (avoid computing it multiple times)
				Ray pinholeRay = camera.generateRayPinhole(u, v);

				for (int apertureSample = 0; apertureSample < numApertureSamples; ++apertureSample) {
					Ray apertureRay = camera.generateRayLens(pinholeRay);
					std::stack<float> refractiveStack;
					pixelSampleColor += traceRay(apertureRay, 0, refractiveStack);
				}

				// Average aperture samples
				pixelSampleColor = pixelSampleColor / static_cast<float>(numApertureSamples);

				// Accumulate pixel samples
				finalColor += pixelSampleColor;
			}

			// Average sample colors
			finalColor = finalColor / static_cast<float>(numPixelSamples);

			// Apply exposure
			float exposure = camera.getExposure();
			if (exposure < 0.5) exposure = 0.5f;	// Ensure exposure is not too low
			finalColor = finalColor * exposure;

			// Tone map to normalize intensities
			float maxIntensity = std::max(finalColor.getR(), std::max(finalColor.getG(), finalColor.getB()));
			if (maxIntensity > 1.0f) {
				finalColor = finalColor.linearToneMap(maxIntensity);
			}

			// Clamp final color to valid range
			finalColor.clamp(0.0f, 1.0f);

			// Save pixel color
			image.setPixelColor(x, y, finalColor);
		}
	}
}


Color Raytracer::traceRay(const Ray& ray, int depth, std::stack<float> refractiveStack) {
	// Base case: Limit the number of bounces
	if (depth > nbounces) {
		return Color(0.0f, 0.0f, 0.0f);  // Black color for exceeded recursion
	}

	// Intersect the scene
	auto bvhRoot = scene.getBVHRoot();
	float t;  // Distance to the closest intersection
	Shape hitObject = scene.traverseBVH(bvhRoot, ray, t, false, 0.0f);
	Color localColor;

	if (rendermode == "binary") {
		if (hitObject.getShapeType() == NO_SHAPE) {
			// No intersection
			return Color(0.0f, 0.0f, 0.0f);  // Black color
		} else {
			// Intersection detected
			return Color(1.0f, 0.0f, 0.0f);  // Red color
		}
	} else if (rendermode == "phong") {
		if (hitObject.getShapeType() == NO_SHAPE) {
			// No intersection
			return scene.getBackgroundColor();
		} else {
			// Intersection detected

			// Retrieve material and intersection details
			Material material = hitObject.getMaterial();
			Vector3 intersectionPoint = ray.pointAtParameter(t);
			Vector3 normal = hitObject.getNormal(intersectionPoint);

			// Local shading using Blinn-Phong
			localColor = shadeBlinnPhong(ray, t, hitObject);

			// Apply texture if available
			if (material.hasTextureMap()) {
				Color textureColor = hitObject.getTextureColor(intersectionPoint, material.getTexture());
				localColor = localColor * (1.0f - material.getKd()) + textureColor * material.getKd();
			}

			// Refraction Logic
			Color refractionColor(0.0f, 0.0f, 0.0f);  // Initialize refraction contribution

			if (material.getIsRefractive() && depth < nbounces) {
				Vector3 viewDir = -(ray.getDirection().normalize());  // View direction
				bool entering = dotProduct(viewDir, normal) > 0;  // Determine if entering or exiting
				Vector3 adjustedNormal = entering ? normal : -normal;

				// Compute refractive indices
				float n1 = refractiveStack.empty() ? 1.0f : refractiveStack.top();
				float n2 = entering ? material.getRefractiveIndex() : (refractiveStack.size() > 1 ? refractiveStack.top() : 1.0f);
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
					}

					// Offset slightly to avoid self-intersection
					Ray refractRay(intersectionPoint - adjustedNormal * 1e-4, refractDir);
					refractionColor = traceRay(refractRay, depth + 1, refractiveStack) * (1.0f - material.getReflectivity());
				}
			}

			// Reflection Logic
			if (material.getIsReflective() && depth < nbounces) {
				/*Vector3 reflectDir = ray.getDirection() - normal * 2.0f * dotProduct(ray.getDirection(), normal);
				reflectDir = reflectDir.normalize();

				Ray reflectRay(intersectionPoint + normal * 1e-4, reflectDir);  // Offset to avoid self-intersection
				*/
				//above code replaced with "Ray reflectRay = ray.reflect(normal, intersectionPoint);" TODO:remove this
				Ray reflectRay = ray.reflect(normal, intersectionPoint);
			 	Color reflectionColor = traceRay(reflectRay, depth + 1, refractiveStack);

				// Combine local, reflected, and refracted colors
				localColor = localColor * (1.0f - material.getReflectivity()) +
							 reflectionColor * material.getReflectivity();
			}
			// Combine local and refracted colors (if no reflection)
			localColor += refractionColor;

			return localColor;
		}
	}
}


Color Raytracer::shadeBlinnPhong(const Ray& ray, float& t, Shape hitObject) {
	Material material = hitObject.getMaterial();;
	Vector3 intersectionPoint = ray.pointAtParameter(t);
	Vector3 n_normal = hitObject.getNormal(intersectionPoint);  // Normal at intersection
	Vector3 v_viewDir = (ray.getOrigin() - intersectionPoint).normalize();


	// Consider ambient light contribution
	Color finalColor = material.getDiffuseColor() * Ka;  // Adjust base ambient light

	for (PointLight light : scene.getPointLights()) {
		Vector3 l_lightDir = (light.getPosition() - intersectionPoint).normalize();
		Color lightIntensity = light.getIntensity();
		float lightDistance = (light.getPosition() - intersectionPoint).norm();

		//Check for shadows
		if (scene.isInShadow(intersectionPoint, l_lightDir, lightDistance, n_normal)) {
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

	// Clamp final color to valid range
	return finalColor.clamp(0.0f, 1.0f);
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

	// Load raytracer settings
	if (j.contains("nBRDFSamples")) {
		numBRDFSamples = j["nBRDFSamples"];
	} else {
		numBRDFSamples = 16;
	}
	std::cout << "nBRDFSamples = " << numBRDFSamples << std::endl;

	if (j.contains("nPixelSamples")) {
		numPixelSamples = j["nPixelSamples"];
	} else {
		numPixelSamples = 1;
	}
	std::cout << "nPixelSamples = " << numPixelSamples << std::endl;

	// Load raytracer settings
	if (j.contains("nApertureSamples")) {
		numApertureSamples = j["nApertureSamples"];
	} else {
		numApertureSamples = 3;
	}
	std::cout << "nApertureSamples = " << numApertureSamples << std::endl;

	// Load camera
	auto camData = j["camera"];
	if (camData["type"] == "pinhole") {
		camera = Camera(
				PINHOLE,
				camData["width"],
				camData["height"],
				Vector3(camData["position"][0], camData["position"][1], camData["position"][2]),
				Vector3(camData["lookAt"][0], camData["lookAt"][1], camData["lookAt"][2]),
				Vector3(camData["upVector"][0], camData["upVector"][1], camData["upVector"][2]),
				camData["fov"],
				camData["exposure"],
				0.0f, 0.0f	// No aperture radius and focal distance in pinhole cameras
		);
	} else if (camData["type"] == "lens") {
		camera = Camera(
				LENS,
				camData["width"],
				camData["height"],
				Vector3(camData["position"][0], camData["position"][1], camData["position"][2]),
				Vector3(camData["lookAt"][0], camData["lookAt"][1], camData["lookAt"][2]),
				Vector3(camData["upVector"][0], camData["upVector"][1], camData["upVector"][2]),
				camData["fov"],
				camData["exposure"],
				camData["apertureRadius"],
				camData["focalDistance"]
		);
	}

	std::cout << "Camera loaded " << "width " << camera.getWidth() << std::endl;
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
		}
	}
	std::cout << "Lights loaded" << std::endl;


	// Load shapes
	for (const auto& shapeData : sceneData["shapes"]) {
		Material material;
		if (shapeData.contains("material")) {
			auto materialData = shapeData["material"];
			if (rendermode == "brdf") {
				material = Material(
						Color(materialData["diffusecolor"][0], materialData["diffusecolor"][1], materialData["diffusecolor"][2]),
						Color(materialData["specularcolor"][0], materialData["specularcolor"][1], materialData["specularcolor"][2]),
						materialData["isreflective"],
						materialData["reflectivity"],
						materialData["isrefractive"],
						materialData["refractiveindex"],
						materialData["roughness"],
						Color(materialData["emission"][0], materialData["emission"][1], materialData["emission"][2])
				);
			} else {	//phong or binary rendermodes
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
			}
			if (materialData.contains("texture")) {
				Image texture = Image(materialData["texture"]);
				material.setTexture(texture);
			}

		} else {
			if (rendermode == "brdf") {
				material = Material(Color(1, 1, 1), Color(1, 1, 1), false, 0.0f, false, 0.0f, 0.0f, Color(0, 0, 0));
			} else {
				material = Material(0.5f, 0.5f, 32, Color(1, 1, 1), Color(1, 1, 1), false, 0.0f, false, 0.0f);
			}
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

	std::cout << "JSON loaded" << std::endl;
	return Image(camera.getWidth(), camera.getHeight());
}


// BRDF methods



void Raytracer::renderBRDFWithoutApertureSampling(Image& image) {
	int width = image.getWidth();
	int height = image.getHeight();

	// Build BVH for the scene
	scene.setBVHRoot(scene.buildBVH(scene.getShapes(), 0, scene.getShapes().size()));

	// Initialize random number generator
	std::random_device rd;
	std::default_random_engine rng(rd());

	// Loop over each pixel
	for (int y = 0; y < height; ++y) {      // Bottom to top
		for (int x = 0; x < width; ++x) {   // Left to right

			Color finalColor(0.0f, 0.0f, 0.0f);

			// ** Pixel Sampling **
			for (int s = 0; s < numPixelSamples; ++s) {
				// Random offset for pixel sampling
				float offset_u = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
				float offset_v = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

				// Normalized pixel sample coordinates
				float u = (static_cast<float>(x) + offset_u) / static_cast<float>(width);
				float v = (static_cast<float>(y) + offset_v) / static_cast<float>(height);

				// Flip u and v if necessary
				u = 1.0f - u;
				v = 1.0f - v;

				// Compute the pinhole ray as usual (avoid computing it multiple times)
				Ray pinholeRay = camera.generateRayPinhole(u, v);

				// Trace the ray using BRDF path tracing
				std::stack<float> refractiveStack;
				Color pixelSampleColor = traceRayBRDFBia(pinholeRay, 0, refractiveStack, rng);

				// Accumulate pixel samples
				finalColor += pixelSampleColor;
			}

			// Average sample colors
			finalColor = finalColor / static_cast<float>(numPixelSamples);

			// Apply exposure
			float exposure = camera.getExposure();
			if (exposure < 0.5f) exposure = 0.5f;    // Ensure exposure is not too low
			finalColor = finalColor * exposure;

			// Tone map to normalize intensities
			float maxIntensity = std::max(finalColor.getR(), std::max(finalColor.getG(), finalColor.getB()));
			if (maxIntensity > 1.0f) {
				finalColor = finalColor.linearToneMap(maxIntensity);
			}

			finalColor.gammaCorrect(2.2f);

			// Clamp final color to valid range
			finalColor.clamp(0.0f, 1.0f);

			// Save pixel color
			image.setPixelColor(x, y, finalColor);
		}
	}
}

void Raytracer::renderBRDFWithApertureSampling(Image& image) {
	int width = image.getWidth();
	int height = image.getHeight();

	// Build BVH for the scene
	scene.setBVHRoot(scene.buildBVH(scene.getShapes(), 0, scene.getShapes().size()));

	// Initialize random number generator
	std::random_device rd;
	std::default_random_engine rng(rd());

	// Loop over each pixel
	for (int y = 0; y < height; ++y) {      // Bottom to top
		for (int x = 0; x < width; ++x) {   // Left to right

			Color finalColor(0.0f, 0.0f, 0.0f);

			// ** Pixel Sampling **
			for (int s = 0; s < numPixelSamples; ++s) {
				// Random offset for pixel sampling
				float offset_u = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
				float offset_v = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

				// Normalized pixel sample coordinates
				float u = (static_cast<float>(x) + offset_u) / static_cast<float>(width);
				float v = (static_cast<float>(y) + offset_v) / static_cast<float>(height);

				// Flip u and v if necessary
				u = 1.0f - u;
				v = 1.0f - v;

				// ** Aperture Sampling (depth of field) **
				Color pixelSampleColor(0.0f, 0.0f, 0.0f);

				// Compute the pinhole ray as usual (avoid computing it multiple times)
				Ray pinholeRay = camera.generateRayPinhole(u, v);

				for (int apertureSample = 0; apertureSample < numApertureSamples; ++apertureSample) {
					Ray apertureRay = camera.generateRayLens(pinholeRay);
					std::stack<float> refractiveStack;

					// Trace the ray using BRDF path tracing
					pixelSampleColor += traceRayBRDF(apertureRay, 0, refractiveStack, rng);
				}

				// Average aperture samples
				pixelSampleColor = pixelSampleColor / static_cast<float>(numApertureSamples);

				// Accumulate pixel samples
				finalColor += pixelSampleColor;
			}

			// Average sample colors
			finalColor = finalColor / static_cast<float>(numPixelSamples);

			// Apply exposure
			float exposure = camera.getExposure();
			if (exposure < 0.5f) exposure = 0.5f;    // Ensure exposure is not too low
			finalColor = finalColor * exposure;

			// Tone map to normalize intensities
			float maxIntensity = std::max(finalColor.getR(), std::max(finalColor.getG(), finalColor.getB()));
			if (maxIntensity > 1.0f) {
				finalColor = finalColor.linearToneMap(maxIntensity);
			}

			// Clamp final color to valid range
			finalColor.clamp(0.0f, 1.0f);

			// Save pixel color
			image.setPixelColor(x, y, finalColor);
		}
	}
}

/*
Color Raytracer::traceRayBRDF(const Ray& ray, int depth, std::stack<float>& refractiveStack, std::default_random_engine& rng) {
	if (depth > nbounces) {
		return Color(0.0f, 0.0f, 0.0f);  // Return black if max depth is exceeded
	}

	// Intersect the scene
	auto bvhRoot = scene.getBVHRoot();
	float t;  // Distance to the closest intersection
	Shape hitObject = scene.traverseBVH(bvhRoot, ray, t, false, 0.0f);

	if (hitObject.getShapeType() == NO_SHAPE) {
		// No intersection
		return scene.getBackgroundColor();
	}

	// Intersection detected
	Vector3 intersectionPoint = ray.pointAtParameter(t);
	Vector3 normal = hitObject.getNormal(intersectionPoint).normalize();

	Material material = hitObject.getMaterial();
	Color emittedRadiance = material.getEmission();

	// Ensure the normal is facing the correct direction
	Vector3 incidentDir = ray.getDirection().normalize();  // From camera to intersection
	if (dotProduct(normal, incidentDir) > 0.0f) {
		normal = -normal;
	}

	// Russian Roulette termination
	float rrProbability = 0.9f;  // Adjust as needed
	if (depth > 5) {
		std::uniform_real_distribution<float> rrDist(0.0f, 1.0f);
		if (rrDist(rng) > rrProbability) {
			return emittedRadiance;
		}
	} else {
		rrProbability = 1.0f;
	}

	Color incomingRadiance(0.0f, 0.0f, 0.0f);
	Color directLighting(0.0f, 0.0f, 0.0f);

	// ** Direct Lighting Calculation with Shadows **
	directLighting = computeDirectLighting(intersectionPoint, normal, -incidentDir, material, rng);

	// ** Material Handling **
	if (material.getIsReflective() || material.getIsRefractive()) {
		// ** Fresnel Factor Calculation **
		float cosThetaI = fabs(dotProduct(normal, -incidentDir));  // Cosine of incident angle

		float etaI = refractiveStack.empty() ? 1.0f : refractiveStack.top();  // Refractive index of incident medium
		float etaT = material.getRefractiveIndex();

		// For reflective materials, use a high refractive index to simulate metals
		if (!material.getIsRefractive()) {
			etaT = 1.5f;  // Example value, adjust based on material
		}

		float reflectance = fresnelDielectric(cosThetaI, etaI, etaT);

		// ** Importance Sampling between Reflection and Refraction **
		std::uniform_real_distribution<float> dist(0.0f, 1.0f);
		float randomSample = dist(rng);

		if (randomSample < reflectance) {
			// ** Reflection **
			Vector3 reflectedDir = reflect(incidentDir, normal).normalize();

			// Microfacet BRDF sampling for reflection
			Color brdf = microfacetBRDF(incidentDir, reflectedDir, normal, material);
			float pdf = microfacetPDF(incidentDir, reflectedDir, normal, material);

			if (pdf > 0.0f) {
				Ray reflectedRay(intersectionPoint + normal * 1e-4f, reflectedDir);
				Color incoming = traceRayBRDF(reflectedRay, depth + 1, refractiveStack, rng);

				incomingRadiance += incoming * brdf * fabs(dotProduct(normal, reflectedDir)) / pdf;
			}
		} else if (material.getIsRefractive()) {
			// ** Refraction **
			bool entering = dotProduct(normal, -incidentDir) > 0.0f;

			float n1 = refractiveStack.empty() ? 1.0f : refractiveStack.top();
			float n2 = entering ? material.getRefractiveIndex() : (refractiveStack.size() > 1 ? refractiveStack.top() : 1.0f);

			float eta = n1 / n2;
			Vector3 refractedDir;

			if (refract(incidentDir, normal, eta, refractedDir)) {
				refractedDir = refractedDir.normalize();

				Ray refractedRay(intersectionPoint - normal * 1e-4f, refractedDir);

				// Update refractive stack
				if (entering) {
					refractiveStack.push(material.getRefractiveIndex());
				} else if (!refractiveStack.empty()) {
					refractiveStack.pop();
				}

				Color incoming = traceRayBRDF(refractedRay, depth + 1, refractiveStack, rng);
				// Assuming no absorption, adjust if material absorbs light
				incomingRadiance += incoming;
			} else {
				// Total internal reflection
				Vector3 reflectedDir = reflect(incidentDir, normal).normalize();
				Ray reflectedRay(intersectionPoint + normal * 1e-4f, reflectedDir);
				Color incoming = traceRayBRDF(reflectedRay, depth + 1, refractiveStack, rng);

				incomingRadiance += incoming;
			}
		} else {
			// For purely reflective materials without refraction
			Vector3 reflectedDir = reflect(incidentDir, normal).normalize();

			// Microfacet BRDF sampling for reflection
			Color brdf = microfacetBRDF(incidentDir, reflectedDir, normal, material);
			float pdf = microfacetPDF(incidentDir, reflectedDir, normal, material);

			if (pdf > 0.0f) {
				Ray reflectedRay(intersectionPoint + normal * 1e-4f, reflectedDir);
				Color incoming = traceRayBRDF(reflectedRay, depth + 1, refractiveStack, rng);

				incomingRadiance += incoming * brdf * fabs(dotProduct(normal, reflectedDir)) / pdf;
			}
		}
	} else {
		// ** Diffuse Material (Lambertian) **
		// Cosine-weighted hemisphere sampling (importance sampling)
		Vector3 sampledDir = cosineSampleHemisphere(normal, rng);

		// Compute BRDF and PDF
		float pdf = fabs(dotProduct(normal, sampledDir)) / M_PI;
		Color brdf = material.getDiffuseColor() / M_PI;

		// Create new ray
		Ray newRay(intersectionPoint + normal * 1e-4f, sampledDir);

		// Recursive trace
		Color incoming = traceRayBRDF(newRay, depth + 1, refractiveStack, rng);

		// Compute contribution
		incomingRadiance = incoming * brdf * fabs(dotProduct(normal, sampledDir)) / pdf;
	}

	// Apply Russian Roulette probability
	incomingRadiance = incomingRadiance / rrProbability;

	// Total radiance is emitted radiance plus direct and indirect lighting
	return emittedRadiance + directLighting + incomingRadiance;
}

// Utility Functions
void Raytracer::sampleBRDF(const Vector3& viewDir, const Vector3& normal, const Material& material,
						   std::default_random_engine& rng, Vector3& sampledDir, float& pdf, Color& brdfValue) {
	if (material.isDiffuse()) {
		// Cosine-weighted hemisphere sampling
		sampledDir = cosineSampleHemisphere(normal, rng);
		pdf = fabs(dotProduct(normal, sampledDir)) / M_PI;
		brdfValue = material.getDiffuseColor() / M_PI;
	} else if (material.getIsReflective()) {
		// Importance sampling for specular reflection
		sampledDir = sampleGGXDistribution(viewDir, normal, material.getRoughness(), rng);
		pdf = computeSpecularPDF(viewDir, sampledDir, normal, material.getRoughness());
		brdfValue = evaluateBRDF(material, viewDir, sampledDir, normal);
	}
	// Handle other material types as needed
}

Vector3 Raytracer::sampleGGXDistribution(const Vector3& viewDir, const Vector3& normal, float roughness, std::default_random_engine& rng) {
	// Generate random numbers
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);
	float r1 = dist(rng);
	float r2 = dist(rng);

	// Sample microfacet normal (half-vector)
	float alpha = roughness * roughness;
	float phi = 2.0f * M_PI * r1;

	float cosTheta = sqrt((1.0f - r2) / (1.0f + (alpha * alpha - 1.0f) * r2 + 1e-7f));
	float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

	float x = sinTheta * cos(phi);
	float y = sinTheta * sin(phi);
	float z = cosTheta;

	// Convert to world space
	Vector3 h;
	// Build an orthonormal basis around the normal
	Vector3 w = normal;
	Vector3 u = crossProduct(fabs(w.x) > 0.1f ? Vector3(0,1,0) : Vector3(1,0,0), w).normalize();
	Vector3 v = crossProduct(w, u);

	h = u * x + v * y + w * z;
	h = h.normalize();

	// Compute the reflected direction
	Vector3 sampledDir = reflect(-viewDir, h);
	return sampledDir.normalize();
}

float Raytracer::computeSpecularPDF(const Vector3& viewDir, const Vector3& sampledDir, const Vector3& normal, float roughness) {
	Vector3 halfVector = (viewDir + sampledDir).normalize();
	float NdotH = std::max(0.0f, dotProduct(normal, halfVector));
	float VdotH = std::max(0.0f, dotProduct(viewDir, halfVector));

	float D = distributionGGX(NdotH, roughness);
	float pdf = D * NdotH / (4.0f * VdotH + 1e-7f);

	return pdf;
}


Color Raytracer::computeDirectLighting(const Vector3& intersectionPoint, const Vector3& normal, const Vector3& viewDir, const Material& material, std::default_random_engine& rng) {
	Color directLight(0.0f, 0.0f, 0.0f);

	// Loop over all light sources
	for (const PointLight& light : scene.getPointLights()) {
		Vector3 lightPos = light.getPosition();
		Vector3 lightDir = (lightPos - intersectionPoint).normalize();
		float lightDist = (lightPos - intersectionPoint).norm();
		float distanceSquared = lightDist * lightDist;


		if (scene.isInShadow(intersectionPoint, lightDir, lightDist, normal)) {
			continue;  // Light is blocked, so skip this light
		}

		// Compute attenuation (if any)
		float attenuation = 1.0f;  // For point lights without attenuation; adjust if needed

		// Compute BRDF value
		float NdotL = std::max(0.0f, dotProduct(normal, lightDir));
		if (NdotL > 0.0f) {
			Color brdfValue = evaluateBRDF(material, -viewDir, lightDir, normal);

			// Light intensity
			Color lightIntensity = light.getIntensity() * attenuation / (distanceSquared + 1e-7);
			//std::cout << "light intensity " << lightIntensity.toString() << " brdfValue " << brdfValue.toString() << " NdotL " << NdotL << std::endl;
			//TODO:remove cout
			// Accumulate the contribution
			directLight += brdfValue * lightIntensity * NdotL;
		}
	}

	return directLight;
}


Color Raytracer::evaluateBRDF(Material material, const Vector3& viewDir, const Vector3& lightDir, const Vector3& normal) const {
	if (material.isDiffuse()) {
		// Diffuse (Lambertian) BRDF
		return material.getDiffuseColor() / M_PI;
	}

	Color brdfValue(0.0f, 0.0f, 0.0f);
	if (material.hasDiffuse()){
		// Diffuse (Lambertian) BRDF
		brdfValue += material.getDiffuseColor() / M_PI;
	}
	if (material.isGlossy() || material.getIsReflective()) {
		// Specular BRDF using microfacet model
		// Compute half-vector
		Vector3 halfVector = (viewDir + lightDir).normalize();
		float NdotH = std::max(0.0f, dotProduct(normal, halfVector));
		float NdotV = std::max(0.0f, dotProduct(normal, viewDir));
		float NdotL = std::max(0.0f, dotProduct(normal, lightDir));
		float VdotH = std::max(0.0f, dotProduct(viewDir, halfVector));

		float roughness = material.getRoughness();

		// Fresnel term
		float F = fresnelDielectric(VdotH, 1.0f, material.getRefractiveIndex());

		// Geometry term
		float G = geometrySmith(NdotV, NdotL, roughness);

		// Normal Distribution Function (NDF)
		float D = distributionGGX(NdotH, roughness);

		// Specular BRDF
		float denom = 4.0f * NdotV * NdotL + 1e-7f;
		Color specular = material.getSpecularColor() * (F * G * D / denom);

		brdfValue += specular;
	}
	return brdfValue;
}


// Fresnel reflectance for dielectric materials using Schlick's approximation
float Raytracer::fresnelDielectric(float cosThetaI, float etaI, float etaT) const {
	float r0 = pow((etaI - etaT) / (etaI + etaT), 2.0f);
	return r0 + (1.0f - r0) * pow(1.0f - cosThetaI, 5.0f);
}

// Cosine-weighted hemisphere sampling
Vector3 Raytracer::cosineSampleHemisphere(const Vector3& normal, std::default_random_engine& rng) {
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);
	float r1 = dist(rng);
	float r2 = dist(rng);

	float phi = 2.0f * M_PI * r1;
	float x = cos(phi) * sqrt(r2);
	float y = sin(phi) * sqrt(r2);
	float z = sqrt(1.0f - r2);

	// Create an orthonormal basis
	Vector3 w = normal;
	Vector3 u = crossProduct(fabs(w.x) > 0.1f ? Vector3(0,1,0) : Vector3(1,0,0), w).normalize();
	Vector3 v = crossProduct(w, u);

	Vector3 sampleDir = u * x + v * y + w * z;
	return sampleDir.normalize();
}

// Microfacet BRDF using Cook-Torrance model with GGX distribution
Color Raytracer::microfacetBRDF(const Vector3& incident, const Vector3& reflected, const Vector3& normal, const Material& material) {
	Vector3 halfVector = (incident + reflected).normalize();
	float NdotH = std::max(0.0f, dotProduct(normal, halfVector));
	float NdotV = std::max(0.0f, dotProduct(normal, -incident));
	float NdotL = std::max(0.0f, dotProduct(normal, reflected));
	float VdotH = std::max(0.0f, dotProduct(-incident, halfVector));

	// Fresnel term
	float F = fresnelDielectric(VdotH, 1.0f, material.getRefractiveIndex());

	// Geometry term
	float G = geometrySmith(NdotV, NdotL, material.getRoughness());

	// Normal Distribution Function (NDF)
	float D = distributionGGX(NdotH, material.getRoughness());

	// Specular BRDF
	float denom = 4.0f * NdotV * NdotL + 1e-7f;
	Color specular = material.getSpecularColor() * (F * G * D / denom);

	return specular;
}

// GGX Normal Distribution Function
float Raytracer::distributionGGX(float NdotH, float roughness) const {
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float denom = (NdotH * NdotH) * (alpha2 - 1.0f) + 1.0f;
	return alpha2 / (M_PI * denom * denom + 1e-7f);
}

// Geometry term using Smith's method
float Raytracer::geometrySmith(float NdotV, float NdotL, float roughness) const {
	float ggx1 = geometrySchlickGGX(NdotV, roughness);
	float ggx2 = geometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
}

// Geometry function for one direction
float Raytracer::geometrySchlickGGX(float NdotV, float roughness) const {
	float r = roughness + 1.0f;
	float k = (r * r) / 8.0f;
	float denom = NdotV * (1.0f - k) + k;
	return NdotV / denom;
}*/

// PDF for microfacet BRDF sampling
float Raytracer::microfacetPDF(const Vector3& viewDir, const Vector3& reflected, const Vector3& normal, const Material& material) {
	Vector3 halfVector = (viewDir + reflected).normalize();
	float NdotH = std::max(0.0f, dotProduct(normal, halfVector));
	float HdotL = std::max(0.0f, dotProduct(halfVector, reflected));

	float D = distributionGGX(NdotH, material.getRoughness());
	float pdf = D * NdotH / (4.0f * HdotL + 1e-7f);
	return pdf;
}




// Reflects the incident direction about the normal
Vector3 Raytracer::reflect(const Vector3& incident, const Vector3& normal) {
	return incident - normal * 2.0f * dotProduct(incident, normal);
}

// Refracts the incident direction through the normal with ratio eta = etaI / etaT
bool Raytracer::refract(const Vector3& incident, const Vector3& normal, float eta, Vector3& refracted) {
	float cosThetaI = dotProduct(-incident, normal);
	float sin2ThetaI = std::max(0.0f, 1.0f - cosThetaI * cosThetaI);
	float sin2ThetaT = eta * eta * sin2ThetaI;

	if (sin2ThetaT >= 1.0f) {
		// Total internal reflection
		return false;
	}

	float cosThetaT = sqrt(1.0f - sin2ThetaT);
	refracted = incident * eta + normal * (eta * cosThetaI - cosThetaT);
	return true;
}

//NEW VERSION


Color Raytracer::traceRayBRDF(const Ray& ray, int depth, std::stack<float>& refractiveStack, std::default_random_engine& rng) {
	if (depth > nbounces) {
		return Color(0.0f, 0.0f, 0.0f);  // Terminate recursion
	}

	// Intersect the scene
	auto bvhRoot = scene.getBVHRoot();
	float t;  // Distance to the closest intersection
	Shape hitObject = scene.traverseBVH(bvhRoot, ray, t, false, 0.0f);

	if (hitObject.getShapeType() == NO_SHAPE) {
		// No intersection
		return scene.getBackgroundColor();
	}

	// Intersection detected
	Vector3 intersectionPoint = ray.pointAtParameter(t);
	Vector3 normal = hitObject.getNormal(intersectionPoint).normalize();

	Material material = hitObject.getMaterial();
	Color emittedRadiance = material.getEmission();

	// Ensure the normal is facing the correct direction
	Vector3 incidentDir = ray.getDirection().normalize();  // From camera to intersection
	if (dotProduct(normal, incidentDir) > 0.0f) {
		normal = -normal;
	}

	// Russian Roulette termination
	float rrProbability = 1.0f;
	if (depth > 5) {
		rrProbability = 0.9f;  // Adjust as needed
		std::uniform_real_distribution<float> rrDist(0.0f, 1.0f);
		if (rrDist(rng) > rrProbability) {
			return emittedRadiance;
		}
	}

	Color incomingRadiance(0.0f, 0.0f, 0.0f);

	// ** Direct Lighting Calculation with Shadows **
	Color directLighting = computeDirectLighting(intersectionPoint, normal, -incidentDir, material, rng);

	if (material.isPerfectMirror()) {
		// ** Perfect Mirror Reflection **
		// Skip Monte Carlo integration and reflect exact ray

		// For purely reflective materials without refraction
		Vector3 reflectedDir = reflect(incidentDir, normal).normalize();
		Ray reflectedRay(intersectionPoint + normal * 1e-4f, reflectedDir);

		Color incoming = traceRayBRDF(reflectedRay, depth + 1, refractiveStack, rng);
		return emittedRadiance + directLighting + incoming / rrProbability;
	}
	// ** Monte Carlo Integration **
	for (int i = 0; i < numBRDFSamples; ++i) {
		Vector3 sampledDir;
		float pdf;
		Color brdfValue;

		// Sample a direction based on the BRDF
		sampleBRDF(-incidentDir, normal, material, rng, refractiveStack, sampledDir, pdf, brdfValue);

		if (pdf > 0.0f && !brdfValue.isZero()) {
			// Create new ray
			Vector3 offsetPoint = intersectionPoint + sampledDir * 1e-4f;
			Ray sampleRay(offsetPoint, sampledDir);

			// Update refractive index stack if necessary
			std::stack<float> refractiveStackCopy = refractiveStack;
			if (material.getIsRefractive()) {
				bool entering = dotProduct(normal, sampledDir) < 0.0f;	//
				if (entering) {
					refractiveStackCopy.push(material.getRefractiveIndex());
				} else if (!refractiveStackCopy.empty()) {
					refractiveStackCopy.pop();
				}
			}

			// Recursive trace
			Color incoming = traceRayBRDF(sampleRay, depth + 1, refractiveStackCopy, rng);

			// Compute contribution
			float cosTheta = std::abs(dotProduct(normal, sampledDir));
			incomingRadiance += incoming * brdfValue * cosTheta / pdf;
		}
		/*
		// ** Material Handling **
		if (material.getIsReflective() || material.getIsRefractive()) {

			float n1 = refractiveStack.empty() ? 1.0f : refractiveStack.top();  // Refractive index of incident medium
			float n2 = material.getRefractiveIndex();

			// For reflective materials, use a high refractive index to simulate metals
			if (!material.getIsRefractive()) {
				n2 = 1.5f;  // Example value, adjust based on material
			}

		 	// ** Fresnel Factor Calculation **
			float cosThetaI = fabs(dotProduct(normal, -incidentDir));  // Cosine of incident angle
		 	// (ratio of reflected vs. refracted light)
			float reflectance = fresnelDielectric(cosThetaI, n1, n2);

			// ** Importance Sampling between Reflection and Refraction **
			std::uniform_real_distribution<float> dist(0.0f, 1.0f);
			float randomSample = dist(rng);

		 	// Choose between reflection and refraction
			if (randomSample < reflectance) {  // Reflection chosen (with probability R)
				// ** Reflection **
				Vector3 reflectedDir = reflect(incidentDir, normal).normalize();

				// Microfacet BRDF sampling for reflection
				Color brdf = microfacetBRDF(incidentDir, reflectedDir, normal, material);
				float pdf = microfacetPDF(incidentDir, reflectedDir, normal, material);

				if (pdf > 0.0f) {
					Ray reflectedRay(intersectionPoint + normal * 1e-4f, reflectedDir);
					Color incoming = traceRayBRDF(reflectedRay, depth + 1, refractiveStack, rng);

					incomingRadiance += incoming * reflectance * brdf * fabs(dotProduct(normal, reflectedDir)) / pdf;
				}
			} else if (material.getIsRefractive()) { // Refraction chosen (with probability 1-R)
				// ** Refraction **
				bool entering = dotProduct(normal, -incidentDir) > 0.0f;

				float n1 = refractiveStack.empty() ? 1.0f : refractiveStack.top();
				float n2 = entering ? material.getRefractiveIndex() : (refractiveStack.size() > 1 ? refractiveStack.top() : 1.0f);

				float eta = n1 / n2;
				Vector3 refractedDir;

				if (refract(incidentDir, normal, eta, refractedDir)) {	// if no Total Internal Reflection
					refractedDir = refractedDir.normalize();

					Ray refractedRay(intersectionPoint - normal * 1e-4f, refractedDir);

					// Update refractive stack
					if (entering) {
						refractiveStack.push(material.getRefractiveIndex());
					} else if (!refractiveStack.empty()) {
						refractiveStack.pop();
					}

					Color incoming = traceRayBRDF(refractedRay, depth + 1, refractiveStack, rng);
					// Assuming no absorption, adjust if material absorbs light
					incomingRadiance += incoming * (1.0f - reflectance);
				} else {	// if Total internal reflection
					// **Total internal reflection **
					Vector3 reflectedDir = reflect(incidentDir, normal).normalize();
					Ray reflectedRay(intersectionPoint + normal * 1e-4f, reflectedDir);
					Color incoming = traceRayBRDF(reflectedRay, depth + 1, refractiveStack, rng);

					incomingRadiance += incoming;
				}
			} else {
				// For purely reflective materials without refraction
				Vector3 reflectedDir = reflect(incidentDir, normal).normalize();

				// Microfacet BRDF sampling for reflection
				Color brdf = microfacetBRDF(incidentDir, reflectedDir, normal, material);
				float pdf = microfacetPDF(incidentDir, reflectedDir, normal, material);

				if (pdf > 0.0f) {
					Ray reflectedRay(intersectionPoint + normal * 1e-4f, reflectedDir);
					Color incoming = traceRayBRDF(reflectedRay, depth + 1, refractiveStack, rng);

					incomingRadiance += incoming * brdf * fabs(dotProduct(normal, reflectedDir)) / pdf;
				}
			}
		} else {
			// ** Diffuse Material (Lambertian) **
			// Cosine-weighted hemisphere sampling (importance sampling)
			Vector3 sampledDir = cosineSampleHemisphere(normal, rng);

			// Compute BRDF and PDF
			float pdf = fabs(dotProduct(normal, sampledDir)) / M_PI;
			Color brdf = material.getDiffuseColor() / M_PI;

			// Create new ray
			Ray newRay(intersectionPoint + normal * 1e-4f, sampledDir);

			// Recursive trace
			Color incoming = traceRayBRDF(newRay, depth + 1, refractiveStack, rng);

			// Compute contribution
			incomingRadiance = incoming * brdf * fabs(dotProduct(normal, sampledDir)) / pdf;
		}*/

	}

	// Average the accumulated radiance
	incomingRadiance = incomingRadiance / static_cast<float>(numBRDFSamples);

	// Apply Russian Roulette probability
	incomingRadiance = incomingRadiance / rrProbability;

	// Total radiance is emitted radiance plus direct and indirect lighting
	return emittedRadiance + directLighting + incomingRadiance;
}

void Raytracer::sampleBRDF(const Vector3& viewDir, const Vector3& normal, const Material& material,
						   std::default_random_engine& rng, std::stack<float>& refractiveStack,
						   Vector3& sampledDir, float& pdf, Color& brdfValue) {
	if (material.isDiffuse()) {
		// ** Diffuse Material Sampling **
		sampledDir = cosineSampleHemisphere(normal, rng);
		pdf = std::abs(dotProduct(normal, sampledDir)) / M_PI;
		brdfValue = material.getDiffuseColor() / M_PI;

	} else if (material.isGlossy() || material.getIsReflective()) {
		// ** Glossy or Reflective Material Sampling **
		sampledDir = sampleGGXDirection(viewDir, normal, material.getRoughness(), rng);
		pdf = computeGGXPDF(viewDir, sampledDir, normal, material);
		brdfValue = evaluateBRDF(material, viewDir, sampledDir, normal);

	} else if (material.getIsRefractive()) {
		// ** Refractive Material Sampling **
		bool entering = dotProduct(normal, /*-*/viewDir) > 0.0f;	//why -viewDir?
		float n1 = refractiveStack.empty() ? 1.0f : refractiveStack.top();
		float n2 = /*material.getRefractiveIndex();*/ entering ? material.getRefractiveIndex() : (refractiveStack.size() > 1 ? refractiveStack.top() : 1.0f);
		//float eta = entering ? (n1 / n2) : (n2 / n1);
		float eta = n1 / n2;
		Vector3 refractedDir;
		if (refract(-viewDir, normal, eta, refractedDir)) {
			sampledDir = refractedDir.normalize();
			pdf = 1.0f;  // Delta distribution
			brdfValue = Color(1.0f, 1.0f, 1.0f);  // Assuming perfect transmission
		} else {
			// ** Total Internal Reflection **
			sampledDir = reflect(-viewDir, normal).normalize();
			pdf = 1.0f;  // Delta distribution
			brdfValue = Color(1.0f, 1.0f, 1.0f);  // Reflectance is usually white
		}

	} else {
		// ** Default Case **
		sampledDir = Vector3(0.0f, 0.0f, 0.0f);
		pdf = 1.0f;
		brdfValue = Color(0.0f, 0.0f, 0.0f);
	}
}

Vector3 Raytracer::sampleGGXDirection(const Vector3& viewDir, const Vector3& normal, float roughness, std::default_random_engine& rng) {
	// Generate random numbers
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);
	float u1 = dist(rng);
	float u2 = dist(rng);

	// Compute alpha (roughness squared)
	float alpha = roughness * roughness;

	// Sample theta and phi
	float phi = 2.0f * M_PI * u1;
	float cosTheta = sqrt((1.0f - u2) / (1.0f + (alpha * alpha - 1.0f) * u2));
	float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

	// Convert spherical coordinates to Cartesian coordinates
	Vector3 halfVector = sphericalDirection(sinTheta, cosTheta, phi);

	// Transform half-vector to world space
	Vector3 tangent, bitangent;
	orthonormalBasis(normal, tangent, bitangent);
	halfVector = (tangent * halfVector.x + bitangent * halfVector.y + normal * halfVector.z).normalize();

	// Compute the reflected direction
	Vector3 sampledDir = reflect(-viewDir, halfVector).normalize();

	return sampledDir;
}

Vector3 Raytracer::sphericalDirection(float sinTheta, float cosTheta, float phi) {
	return Vector3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
}

float Raytracer::computeGGXPDF(const Vector3& viewDir, const Vector3& sampledDir, const Vector3& normal, const Material& material) {
	Vector3 incidentDir = -viewDir;
	/*Vector3 halfVector = (sampledDir - incidentDir).normalize();
	float NdotH = std::max(0.0f, dotProduct(normal, halfVector));
	float VdotH = std::max(0.0f, dotProduct(viewDir, halfVector));

	float D = distributionGGX(NdotH, material.getRoughness());
	float pdf = D * NdotH / (4.0f * VdotH + 1e-7f);

	return pdf;*/
	return microfacetPDF(incidentDir, sampledDir, normal, material);
}

Color Raytracer::evaluateBRDF(const Material& material, const Vector3& viewDir, const Vector3& lightDir, const Vector3& normal) const {
	Color brdfValue(0.0f, 0.0f, 0.0f);

	if (material.isDiffuse()) {
		// ** Diffuse BRDF **
		return material.getDiffuseColor() / M_PI;
	}


	if (material.hasDiffuse()){
		// Diffuse (Lambertian) BRDF
		brdfValue += material.getDiffuseColor() / M_PI;
	}

	if (material.isGlossy() || material.getIsReflective()) {
		// ** Specular BRDF using Microfacet Model **
		Color specular = microfacetBRDF(viewDir, lightDir, normal, material);
		/*
		Vector3 halfVector = (viewDir + lightDir).normalize();
		float NdotH = std::max(0.0f, dotProduct(normal, halfVector));
		float NdotV = std::max(0.0f, dotProduct(normal, viewDir));
		float NdotL = std::max(0.0f, dotProduct(normal, lightDir));
		float VdotH = std::max(0.0f, dotProduct(viewDir, halfVector));

		float roughness = material.getRoughness();

		// Fresnel term
		float F = fresnelDielectric(VdotH, 1.0f, material.getRefractiveIndex());

		// Geometry term
		float G = geometrySmith(NdotV, NdotL, roughness);

		// Normal Distribution Function (NDF)
		float D = distributionGGX(NdotH, roughness);

		// Specular BRDF
		float denom = 4.0f * NdotV * NdotL + 1e-7f;
		Color specular = material.getSpecularColor() * (F * G * D / denom);
		*/
		brdfValue += specular;
	}

	// Refractive materials are handled separately

	return brdfValue;
}

float Raytracer::fresnelDielectric(float cosThetaI, float etaI, float etaT) const {
	// Clamp cosThetaI to [-1, 1]
	cosThetaI = std::clamp(cosThetaI, -1.0f, 1.0f);

	// Swap indices of refraction if needed
	bool entering = cosThetaI > 0.0f;
	if (!entering) {
		std::swap(etaI, etaT);
		cosThetaI = fabs(cosThetaI);
	}

	// Compute sinThetaT using Snell's Law
	float sinThetaI = sqrt(std::max(0.0f, 1.0f - cosThetaI * cosThetaI));
	float sinThetaT = etaI / etaT * sinThetaI;

	// Total internal reflection
	if (sinThetaT >= 1.0f) {
		return 1.0f;
	}

	float cosThetaT = sqrt(std::max(0.0f, 1.0f - sinThetaT * sinThetaT));

	float Rparl = ((etaT * cosThetaI) - (etaI * cosThetaT)) / ((etaT * cosThetaI) + (etaI * cosThetaT));
	float Rperp = ((etaI * cosThetaI) - (etaT * cosThetaT)) / ((etaI * cosThetaI) + (etaT * cosThetaT));
	return (Rparl * Rparl + Rperp * Rperp) / 2.0f;
}

void Raytracer::orthonormalBasis(const Vector3& normal, Vector3& tangent, Vector3& bitangent) {
	if (fabs(normal.x) > fabs(normal.z)) {
		tangent = Vector3(-normal.y, normal.x, 0.0f).normalize();
	} else {
		tangent = Vector3(0.0f, -normal.z, normal.y).normalize();
	}
	bitangent = crossProduct(normal, tangent);
}


Vector3 Raytracer::cosineSampleHemisphere(const Vector3& normal, std::default_random_engine& rng) {
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);
	float u1 = dist(rng);
	float u2 = dist(rng);

	float r = sqrt(u1);

	float theta = 2.0f * M_PI * u2;
	float x = r * cos(theta);
	float y = r * sin(theta);
	float z = sqrt(1.0f - u1);

	// Convert to world space
	Vector3 tangent, bitangent;
	orthonormalBasis(normal, tangent, bitangent);

	Vector3 sampleDir = tangent * x + bitangent * y + normal * z;
	return sampleDir.normalize();
}

float Raytracer::distributionGGX(float NdotH, float roughness) const {
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float denom = (NdotH * NdotH) * (alpha2 - 1.0f) + 1.0f;
	return alpha2 / (M_PI * denom * denom + 1e-7f);
}

float Raytracer::geometrySmith(float NdotV, float NdotL, float roughness) const {
	float ggx1 = geometrySchlickGGX(NdotV, roughness);
	float ggx2 = geometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
}

float Raytracer::geometrySchlickGGX(float NdotV, float roughness) const {
	float k = (roughness * roughness) / 2.0f;
	return NdotV / (NdotV * (1.0f - k) + k);
}

Color Raytracer::computeDirectLighting(const Vector3& intersectionPoint, const Vector3& normal,
									   const Vector3& viewDir, const Material& material,
									   std::default_random_engine& rng) {
	Color directLight(0.0f, 0.0f, 0.0f);

	// Number of samples for area lights (set to 1 for point lights)
	int numLightSamples = 1;

	// Loop over all light sources
	for (const PointLight& light : scene.getPointLights()) {
		Color lightContribution(0.0f, 0.0f, 0.0f);

		// ** Handle Different Light Types **
		/*if (light.getType() == LightType::POINT) {
			// Point Light
			numLightSamples = 1;
		} else if (light.getType() == LightType::AREA) {
			// Area Light
			numLightSamples = light.getSampleCount();  // Adjust as needed
		}*/

		// ** Multiple Importance Sampling **
		for (int i = 0; i < numLightSamples; ++i) {
			Vector3 lightPosition;
			Vector3 lightNormal;
			float pdfLight = 1.0f;

			// Sample a point on the light source
			light.sampleLight(intersectionPoint, rng, lightPosition, lightNormal, pdfLight);

			Vector3 lightDir = (lightPosition - intersectionPoint).normalize();
			float lightDistance = (lightPosition - intersectionPoint).norm();
			float distanceSquared = lightDistance * lightDistance;

			// Check if the point is in shadow
			if (scene.isInShadow(intersectionPoint, lightDir, lightDistance, normal)) {
				continue;  // Light is blocked
			}

			// ** Compute BRDF Value **
			float NdotL = std::max(0.0f, dotProduct(normal, lightDir));
			if (NdotL > 0.0f) {
				Color brdfValue = evaluateBRDF(material, viewDir, lightDir, normal);

				// ** Light Emission and Attenuation **
				Color lightIntensity = light.getIntensity(intersectionPoint, lightPosition, lightNormal);

				// ** Geometry Term for Area Lights **
				float G = 1.0f;
				/*if (light.getType() == LightType::AREA) {
					float NdotL_light = std::max(0.0f, dotProduct(lightNormal, -lightDir));
					G = NdotL_light * NdotL / (distanceSquared + 1e-7f);
				} else {*/
					// For point lights, use inverse square law
					lightIntensity = lightIntensity / (distanceSquared + 1e-7f);
				//}

				// ** Accumulate Contribution **
				lightContribution += brdfValue * lightIntensity * NdotL * G / pdfLight;
			}
		}

		// Average over the number of samples
		directLight += lightContribution / static_cast<float>(numLightSamples);
	}

	return directLight;
}

//test

// Microfacet BRDF using Cook-Torrance model with GGX distribution
Color Raytracer::microfacetBRDF(const Vector3& viewDir, const Vector3& reflected, const Vector3& normal, const Material& material) const {
	Vector3 halfVector = (-viewDir + reflected).normalize();	//NOTE debug should this be viewDir ?
	float NdotH = std::max(0.0f, dotProduct(normal, halfVector));
	float NdotV = std::max(0.0f, dotProduct(normal, viewDir));
	float NdotL = std::max(0.0f, dotProduct(normal, reflected));
	float VdotH = std::max(0.0f, dotProduct(viewDir, halfVector));

	// Fresnel term
	float F = fresnelDielectric(VdotH, 1.0f, material.getRefractiveIndex());

	// Geometry term
	float G = geometrySmith(NdotV, NdotL, material.getRoughness());

	// Normal Distribution Function (NDF)
	float D = distributionGGX(NdotH, material.getRoughness());

	// Specular BRDF
	float denom = 4.0f * NdotV * NdotL + 1e-7f;
	Color specular = material.getSpecularColor() * (F * G * D / denom);

	return specular;
}



Color Raytracer::traceRayBRDFBia(const Ray& ray, int depth, std::stack<float>& refractiveStack, std::default_random_engine& rng) {
	if (depth > nbounces) {
		return Color(0.0f, 0.0f, 0.0f);  // Terminate recursion
	}

	// Intersect the scene
	auto bvhRoot = scene.getBVHRoot();
	float t;  // Distance to the closest intersection
	Shape hitObject = scene.traverseBVH(bvhRoot, ray, t, false, 0.0f);

	if (hitObject.getShapeType() == NO_SHAPE) {
		// No intersection
		return scene.getBackgroundColor();
	}

	// Intersection detected
	Vector3 intersectionPoint = ray.pointAtParameter(t);
	Vector3 normal = hitObject.getNormal(intersectionPoint).normalize();

	Material material = hitObject.getMaterial();
	Color emittedRadiance = material.getEmission();

	// Ensure the normal is facing the correct direction
	Vector3 incidentDir = ray.getDirection().normalize();  // From camera to intersection
	Vector3 viewDir = -incidentDir;
	if (dotProduct(normal, incidentDir) > 0.0f) {
		normal = -normal;
	}

	// Russian Roulette termination
	float rrProbability = 1.0f;
	if (depth > 5) {
		rrProbability = 0.9f;  // Adjust as needed
		std::uniform_real_distribution<float> rrDist(0.0f, 1.0f);
		if (rrDist(rng) > rrProbability) {
			return emittedRadiance;
		}
	}

	Color incomingRadiance(0.0f, 0.0f, 0.0f);

	// ** Direct Lighting Calculation with Shadows **
	Color directLighting = computeDirectLighting(intersectionPoint, normal, viewDir, material, rng);

	if (material.isPerfectMirror()) {
		// ** Perfect Mirror Reflection **
		// Skip Monte Carlo integration and reflect exact ray
		// For purely reflective materials without refraction
		Vector3 reflectedDir = reflect(incidentDir, normal).normalize();
		Ray reflectedRay(intersectionPoint + normal * 1e-4f, reflectedDir);

		Color incoming = traceRayBRDF(reflectedRay, depth + 1, refractiveStack, rng);
		return emittedRadiance + directLighting + incoming / rrProbability;
	}

	// ** Monte Carlo Integration **
	int numValidBRDFSamples = numBRDFSamples;
	for (int i = 0; i < numBRDFSamples; ++i) {
		Vector3 sampledDir;
		float pdf;
		Color brdf;

		if (material.getIsRefractive()){
			// ** Refractive Material Sampling **

			float cosThetaI = fabs(dotProduct(normal, viewDir));  // Cosine of incident angle
			bool entering = dotProduct(normal, viewDir) > 0.0f;	//why -viewDir?
			float n1 = refractiveStack.empty() ? 1.0f : refractiveStack.top();
			float n2 =  entering ? material.getRefractiveIndex() :
						(refractiveStack.size() > 1 ? refractiveStack.top() : 1.0f);

			// Fresnel Factor Calculation
			// (ratio of reflected vs. refracted light)
			float reflectance = fresnelDielectric(cosThetaI, n1, n2);

			// ** Importance Sampling between Reflection and Refraction **
			std::uniform_real_distribution<float> dist(0.0f, 1.0f);
			float randomSample = dist(rng);

			// Choose between reflection and refraction
			if (randomSample < reflectance) {
				// ** Reflection chosen (with probability R) **
				// Sample a reflection direction
				sampleBRDFBia(viewDir, normal, material, rng, refractiveStack, sampledDir, pdf, brdf);

				if (pdf > 0.0f && !brdf.isZero()) {
					Ray reflectedRay(intersectionPoint + normal * 1e-4f, sampledDir);
					Color incoming = traceRayBRDFBia(reflectedRay, depth + 1, refractiveStack, rng);

					// Compute contribution
					float cosTheta = std::abs(dotProduct(normal, sampledDir));
					incomingRadiance += incoming * reflectance * brdf * cosTheta / pdf;
				} else {
					numValidBRDFSamples--;
				}
			} else {
				// ** Refraction chosen (with probability 1-R) **
				// Deterministic refraction
				float eta = n1 / n2;

				if (refract(incidentDir, normal, eta, sampledDir)) {	// if no Total Internal Reflection
					sampledDir = sampledDir.normalize();

					Ray refractedRay(intersectionPoint - normal * 1e-4f, sampledDir);

					std::stack<float> refractiveStackCopy = refractiveStack;
					// Update refractive stack
					if (entering) {
						refractiveStackCopy.push(material.getRefractiveIndex());
					} else if (!refractiveStackCopy.empty()) {
						refractiveStackCopy.pop();
					}

					Color incoming = traceRayBRDFBia(refractedRay, depth + 1, refractiveStackCopy, rng);

					incomingRadiance += incoming * (1.0f - reflectance);

				} else {
					// **Total internal reflection **
					// Deterministic reflection like the perfect mirror
					sampledDir = reflect(incidentDir, normal).normalize();
					Ray reflectedRay(intersectionPoint + normal * 1e-4f, sampledDir);
					Color incoming = traceRayBRDFBia(reflectedRay, depth + 1, refractiveStack, rng);

					incomingRadiance += incoming; //reflectance = 1
				}
			}
		} else {
			// ** Glossy or Diffuse Material Sampling **
			// Sample a direction based on the BRDF
			// Sample a direction based on the BRDF
			sampleBRDFBia(viewDir, normal, material, rng, refractiveStack, sampledDir, pdf, brdf);

			if (pdf > 0.0f && !brdf.isZero()) {
				// Create new ray
				Ray sampleRay(intersectionPoint + sampledDir * 1e-4f, sampledDir);

				// Recursive trace
				std::stack<float> refractiveStackCopy = refractiveStack;
				Color incoming = traceRayBRDFBia(sampleRay, depth + 1, refractiveStackCopy, rng);

				// Compute contribution
				float cosTheta = std::abs(dotProduct(normal, sampledDir));
				incomingRadiance += incoming * brdf * cosTheta / pdf;
			}  else {
				numValidBRDFSamples--;
			}
		}
	}

	// Average the accumulated radiance
	incomingRadiance = incomingRadiance / static_cast<float>(numValidBRDFSamples);

	// Apply Russian Roulette probability
	incomingRadiance = incomingRadiance / rrProbability;

	// Total radiance is emitted radiance plus direct and indirect lighting
	return emittedRadiance + directLighting + incomingRadiance;
}


void Raytracer::sampleBRDFBia(const Vector3& viewDir, const Vector3& normal, const Material& material,
						   std::default_random_engine& rng, std::stack<float>& refractiveStack,
						   Vector3& sampledDir, float& pdf, Color& brdfValue) {
	if (material.isDiffuse()) {
		// ** Diffuse Material Sampling **
		sampledDir = cosineSampleHemisphere(normal, rng);
		pdf = std::abs(dotProduct(normal, sampledDir)) / M_PI;
		brdfValue = material.getDiffuseColor() / M_PI;

	} else if (material.isGlossy() || material.getIsReflective()) {
		// ** Glossy or Reflective Material Sampling **
		sampledDir = sampleGGXDirection(viewDir, normal, material.getRoughness(), rng);
		pdf = computeGGXPDF(viewDir, sampledDir, normal, material);
		brdfValue = evaluateBRDF(material, viewDir, sampledDir, normal);

	} else {
		// ** Default Case **
		sampledDir = Vector3(0.0f, 0.0f, 0.0f);
		pdf = 1.0f;
		brdfValue = Color(0.0f, 0.0f, 0.0f);
	}
}
