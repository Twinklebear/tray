#include "linalg/ray.h"
#include "geometry/differential_geometry.h"
#include "geometry/geometry.h"
#include "scene.h"
#include "integrator/surface_integrator.h"
#include "renderer/renderer.h"

Renderer::Renderer(std::unique_ptr<SurfaceIntegrator> surface_integrator)
	: surface_integrator(std::move(surface_integrator))
{}
Colorf Renderer::illumination(RayDifferential &ray, const Scene &scene, std::minstd_rand &rng) const {
	DifferentialGeometry dg;
	if (scene.get_root().intersect(ray, dg)){
		dg.compute_differentials(ray);
		return surface_integrator->illumination(scene, *this, ray, dg, rng);
	}
	else if (scene.get_environment()){
		//TODO: Compute light along the ray coming from lights
		DifferentialGeometry dg;
		dg.point = Point{ray.d.x, ray.d.y, ray.d.z};
		return scene.get_environment()->sample(dg);
	}
	return Colorf{0};
}

