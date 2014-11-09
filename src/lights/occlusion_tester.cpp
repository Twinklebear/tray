#include "scene.h"
#include "geometry/differential_geometry.h"
#include "lights/occlusion_tester.h"

void OcclusionTester::set_points(const Point &a, const Point &b){
	ray = Ray{a, b - a, 1e-4f, 1};
}
void OcclusionTester::set_ray(const Point &p, const Vector &d){
	ray = Ray{p, d.normalized(), 1e-4f};
}
bool OcclusionTester::occluded(const Scene &scene){
	DifferentialGeometry dg;
	return scene.get_root().intersect(ray, dg);
}

