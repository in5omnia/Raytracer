#ifndef RAYTRACER_RAYTRACER_H
#define RAYTRACER_RAYTRACER_H
#include "json.hpp"
#include <fstream>
#include <cstdlib>
#include <random>  // For random number generation
//#include <omp.h>
#include <stack>  // Include this at the top of your file
#include "Image.h"
#include "Scene.h"
#include "Camera.h"
#include "Ray.h"
#include "Color.h"
#include "Scene.h"
#include "Shape.h"
#include "Light.h"
#include "Material.h"
#include "BVH.h"


#define Ka 0.2f

class Raytracer {
	private:
		int nbounces;
		int numPixelSamples;
		int numApertureSamples;
		int numBRDFSamples;	// number of Monte Carlo samples
		std::string rendermode;
	Camera camera;
		Scene scene;

	public:
		Raytracer();
		void render(Image& image);
		void renderWithApertureSampling(Image& image);
		Color traceRay(const Ray& ray, int depth, std::stack<float> refractiveStack);
		Color shadeBlinnPhong(const Ray& ray, float& t, Shape hitObject);

		//BRDF methods
		/*void renderBRDFWithApertureSampling(Image& image);
		Color traceRayBRDF(const Ray& ray, int depth, std::stack<float>& refractiveStack);
		Color computeDirectLighting(const Vector3& hitPoint, const Vector3& normal, const Material& material, const Vector3& viewDir);
		Vector3 sampleBRDF(const Vector3& normal, const Vector3& viewDir, const Material& material);
		Vector3 sampleCosineHemisphere(const Vector3& normal);
		Vector3 sampleMicrofacet(const Vector3& normal, float roughness);

		Vector3 sampleAroundVector(const Vector3& direction, float roughness);
		float samplePDF(const Vector3& sampledDir, const Vector3& viewDir, const Vector3& normal, const Material& material);

		Color evaluateBRDF(const Vector3& normal, const Vector3& viewDir, const Vector3& lightDir, const Material& material);
		Color computeFresnel(const Vector3& viewDir, const Vector3& halfway, const Color& specularColor);
		float computeMicrofacetDistribution(const Vector3& halfway, const Vector3& normal, float roughness);
		float computeGeometryTerm(const Vector3& rayDir, const Vector3& lightDir, const Vector3& normal, float roughness);
		float fresnelDielectric(float cosTheta, float n1, float n2);*/
		void renderBRDFWithoutApertureSampling(Image& image);
		void renderBRDFWithApertureSampling(Image& image);
		Color traceRayBRDF(const Ray& ray, int depth, std::stack<float>& refractiveStack, std::default_random_engine& rng);
		float fresnelDielectric(float cosThetaI, float etaI, float etaT) const;
		Vector3 cosineSampleHemisphere(const Vector3& normal, std::default_random_engine& rng);
		Color microfacetBRDF(const Vector3& incident, const Vector3& reflected, const Vector3& normal, const Material& material);
		float distributionGGX(float NdotH, float roughness) const;
		float geometrySmith(float NdotV, float NdotL, float roughness) const ;
		float geometrySchlickGGX(float NdotV, float roughness) const;
		float microfacetPDF(const Vector3& incident, const Vector3& reflected, const Vector3& normal, const Material& material);
		Color evaluateBRDF(Material material, const Vector3& viewDir, const Vector3& lightDir, const Vector3& normal) const;

		//new
		Color computeDirectLighting(const Vector3& intersectionPoint, const Vector3& normal, const Vector3& viewDir, const Material& material, std::default_random_engine& rng);
		void sampleBRDF(const Vector3& viewDir, const Vector3& normal, const Material& material,
					   	std::default_random_engine& rng, Vector3& sampledDir, float& pdf, Color& brdfValue);
		Vector3 sampleGGXDistribution(const Vector3& viewDir, const Vector3& normal, float roughness, std::default_random_engine& rng);
		float computeSpecularPDF(const Vector3& viewDir, const Vector3& sampledDir, const Vector3& normal, float roughness);

		//refraction and reflection methods
		bool refract(const Vector3& I, const Vector3& N, float eta, Vector3& T);
		Vector3 reflect(const Vector3& I, const Vector3& N);

		//read json method
		Image readJSON(const std::string& filename);

};


#endif //RAYTRACER_RAYTRACER_H
