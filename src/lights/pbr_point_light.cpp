#include "linalg/util.h"
#include "lights/pbr_point_light.h"

PBRPointLight::PBRPointLight(const Transform &to_world, const Colorf &intensity)
	: PBRLight(to_world), position(to_world(Point{0, 0, 0})), intensity(intensity)
{}
Colorf PBRPointLight::sample(const Point &p, const std::array<float, 2>&,
	Vector &wi, float &pdf_val, OcclusionTester &occlusion) const
{
	wi = (position - p).normalized();
	pdf_val = 1;
	occlusion.set_points(p, position);
	return intensity / position.distance_sqr(p);
}
Colorf PBRPointLight::power(const Scene&) const {
	return 4 * PI * intensity;
}
bool PBRPointLight::delta_light() const {
	return true;
}
float PBRPointLight::pdf(const Point&, const Vector&) const {
	return 0;
}

