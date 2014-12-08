#include "scene.h"
#include "integrator/emission_integrator.h"

EmissionIntegrator::EmissionIntegrator(float step_size) : step_size(step_size){}
Colorf EmissionIntegrator::radiance(const Scene &scene, const Renderer&, const RayDifferential &r,
	Sampler &sampler, MemoryPool&, Colorf &transmit) const
{
	const VolumeNode *vol = scene.get_volume_root();
	Ray ray = r;
	if (vol == nullptr || !vol->intersect(ray) || ray.min_t == ray.max_t){
		transmit = Colorf{1};
		return Colorf{0};
	}
	Colorf rad;
	//We integrate through the volume via ray marching with steps of size step_size
	int n_samples = std::ceil((ray.max_t - ray.min_t) / step_size);
	float step = (ray.max_t - ray.min_t) / n_samples;
	transmit = Colorf{1};
	Point p = ray(ray.min_t), p_prev;
	Vector w_o = -ray.d;
	//Our first point inside the volume is offset by some random value
	float t = ray.min_t + sampler.random_float() * step;
	//Step through the volume
	for (int i = 0; i < n_samples; ++i, t += step, p_prev = p){
		p = ray(t);
		//Step forward and update the transmittance to include contrubition from this segment
		Ray step_ray{p_prev, p - p_prev, ray, 0, 1};
		Colorf step_tau = -vol->optical_thickness(step_ray, 0.5 * step_size, sampler.random_float());
		transmit *= step_tau.exp();

		//We consider terminating the ray if the transmittance is very low
		if (transmit.luminance() < 1e-3){
			const float continue_prob = 0.5;
			if (sampler.random_float() > continue_prob){
				transmit = 0;
				break;
			}
			transmit /= continue_prob;
		}
		//Add in emission contribution from emissive volumes
		rad += transmit * vol->emission(p, w_o);
	}
	return rad * step;
}
Colorf EmissionIntegrator::transmittance(const Scene &scene, const Renderer&, const RayDifferential &ray,
	Sampler &sampler, MemoryPool&) const
{
	const VolumeNode *vol = scene.get_volume_root();
	if (vol == nullptr){
		return Colorf{1};
	}
	Colorf tau = -vol->optical_thickness(ray, step_size, sampler.random_float());
	return tau.exp();
}

