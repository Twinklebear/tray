#include "lights/occlusion_tester.h"

void OcclusionTester::set_points(const Point &a, const Point &b){
	float dist = a.distance(b);
	ray = Ray{a, (b - a) / dist, 1e-6f, dist * (1.f - 1e-6f)};
}
void OcclusionTester::set_ray(const Point &p, const Vector &d){
	ray = Ray{p, d.normalized(), 1e-6f};
}
bool OcclusionTester::occluded(const Scene &scene){
	DifferentialGeometry dg;
	return scene.get_root().intersect(ray, dg);
}

