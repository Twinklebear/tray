#include "scene.h"
#include "lights/area_light.h"

AreaLight::AreaLight(const Transform &to_world, const Colorf &emit, int n_samples, float radius)
	: PBRLight(to_world, n_samples), emit(emit), geometry(std::make_unique<Sphere>(radius)),
	surface_area(geometry->surface_area())
{}
Colorf AreaLight::sample(const Point &p, const std::array<float, 2> &lsample,
	Vector &wi, float &pdf_val, OcclusionTester &occlusion) const
{
	return Colorf{0};
}
Colorf AreaLight::sample(const Scene &scene, const std::array<float, 2> &a, const std::array<float, 2> &b,
	Ray &ray, Normal &normal, float &pdf_val) const
{
}
Colorf AreaLight::power(const Scene &scene) const {
	return Colorf{0};
}
bool AreaLight::delta_light() const {
	return false;
}
float AreaLight::pdf(const Point &p, const Vector &wi) const {
	return 0;
}
Colorf AreaLight::radiance(const Point &p, const Normal &n, const Vector &w) const {
	return w.dot(n) > 0 ? emit : Colorf{0};
}

