#include "scene.h"
#include "geometry/differential_geometry.h"
#include "lights/occlusion_tester.h"

void OcclusionTester::set_points(const Point &a, const Point &b){
	ray = Ray{a, b - a, 0.001, 0.999};
}
void OcclusionTester::set_ray(const Point &p, const Vector &d){
	ray = Ray{p, d.normalized(), 0.001};
}
bool OcclusionTester::occluded(const Scene &scene){
	DifferentialGeometry dg;
	return scene.get_root().intersect(ray, dg);
}
Colorf OcclusionTester::transmittance(const Scene &scene, const Renderer &renderer, Sampler &sampler,
	MemoryPool &pool)
{
	return renderer.transmittance(scene, RayDifferential{ray}, sampler, pool);
}

