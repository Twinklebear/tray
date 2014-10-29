#include "linalg/ray.h"
#include "geometry/differential_geometry.h"
#include "geometry/geometry.h"
#include "scene.h"
#include "integrator/surface_integrator.h"
#include "renderer/renderer.h"

Renderer::Renderer(std::unique_ptr<SurfaceIntegrator> surface_integrator)
	: surface_integrator(std::move(surface_integrator))
{}
Colorf Renderer::illumination(RayDifferential &ray, const Scene &scene) const {
	DifferentialGeometry dg;
	if (scene.get_root().intersect(ray, dg)){
		dg.compute_differentials(ray);
		return surface_integrator->illumination(scene, *this, ray, dg);
	}
	else {
		//TODO: Compute light along the ray coming from lights
		return Colorf{0};
	}
}

