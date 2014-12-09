#include "linalg/ray.h"
#include "geometry/differential_geometry.h"
#include "geometry/geometry.h"
#include "scene.h"
#include "integrator/surface_integrator.h"
#include "integrator/volume_integrator.h"
#include "integrator/emission_integrator.h"
#include "integrator/single_scattering_integrator.h"
#include "renderer/renderer.h"

Renderer::Renderer(std::unique_ptr<SurfaceIntegrator> surface_integrator)
	: surface_integrator(std::move(surface_integrator)), volume_integrator(std::make_unique<SingleScatteringIntegrator>(0.5f))
{}
void Renderer::preprocess(const Scene &scene){
	surface_integrator->preprocess(scene);
}
Colorf Renderer::illumination(RayDifferential &ray, const Scene &scene, Sampler &sampler, MemoryPool &pool) const {
	DifferentialGeometry dg;
	Colorf illum;
	if (scene.get_root().intersect(ray, dg)){
		illum = surface_integrator->illumination(scene, *this, ray, dg, sampler, pool);
	}
	else if (scene.get_environment()){
		//TODO: Compute light along the ray coming from lights
		DifferentialGeometry dg;
		dg.point = Point{ray.d.x, ray.d.y, ray.d.z};
		illum = scene.get_environment()->sample(dg);
	}
	Colorf vol_radiance, transmit{1};
	if (volume_integrator != nullptr){
		vol_radiance = volume_integrator->radiance(scene, *this, ray, sampler, pool, transmit);
	}
	return transmit * illum + vol_radiance;
}
Colorf Renderer::transmittance(const Scene &scene, const RayDifferential &ray, Sampler &sampler, MemoryPool &pool) const {
	return volume_integrator != nullptr ? volume_integrator->transmittance(scene, *this, ray, sampler, pool) : Colorf{1};
}

