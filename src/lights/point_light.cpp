#include "linalg/util.h"
#include "monte_carlo/util.h"
#include "lights/point_light.h"

PointLight::PointLight(const Transform &to_world, const Colorf &intensity)
	: Light(to_world), position(to_world(Point{0, 0, 0})), intensity(intensity)
{}
Colorf PointLight::sample(const Point &p, const LightSample&, Vector &w_i, float &pdf_val, OcclusionTester &occlusion) const {
	w_i = (position - p).normalized();
	pdf_val = 1;
	occlusion.set_points(p, position);
	return intensity / position.distance_sqr(p);
}
Colorf PointLight::sample(const Scene&, const LightSample &lsample, const std::array<float, 2>&,
	Ray &ray, Normal &normal, float &pdf_val) const
{
	ray = Ray{position, uniform_sample_sphere(lsample.u)};
	normal = Normal{ray.d};
	pdf_val = uniform_sphere_pdf();
	return intensity;
}
Colorf PointLight::power(const Scene&) const {
	return 4 * PI * intensity;
}
bool PointLight::delta_light() const {
	return true;
}
float PointLight::pdf(const Point&, const Vector&) const {
	return 0;
}

