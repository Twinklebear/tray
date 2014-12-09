#include "scene.h"
#include "integrator/single_scattering_integrator.h"

SingleScatteringIntegrator::SingleScatteringIntegrator(float step_size) : step_size(step_size){}
Colorf SingleScatteringIntegrator::radiance(const Scene &scene, const Renderer &renderer, const RayDifferential &ray,
	Sampler &sampler, MemoryPool &pool, Colorf &transmit) const
{
	const VolumeNode *vol = scene.get_volume_root();
	std::array<float, 2> t_range;
	if (vol == nullptr || !vol->intersect(ray, t_range) || t_range[0] == t_range[1]){
		transmit = Colorf{1};
		return Colorf{0};
	}
	Colorf rad;
	//We integrate through the volume via ray marching with steps of size step_size
	int n_samples = std::ceil((t_range[1] - t_range[0]) / step_size);
	float step = (t_range[1] - t_range[0]) / n_samples;
	transmit = Colorf{1};
	Point p = ray(t_range[0]), p_prev;
	Vector w_o = -ray.d;
	//Our first point inside the volume is offset by some random value
	float t = t_range[0] + sampler.random_float() * step;

	//Setup samples for uniformly sampling lights to consider in scattering effects from
	auto *l_samples_u = pool.alloc_array<std::array<float, 2>>(n_samples);
	auto *l_samples_comp = pool.alloc_array<float>(n_samples);
	auto *l_samples_num = pool.alloc_array<float>(n_samples);
	sampler.get_samples(l_samples_u, n_samples);
	sampler.get_samples(l_samples_comp, n_samples);
	sampler.get_samples(l_samples_num, n_samples);

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

		//Compute single in scattering effects by choosing a light and finding how much is
		//scattered back along the incident ray
		Colorf scatter = vol->scattering(p, w_o);
		if (!scatter.is_black() && !scene.get_light_cache().empty()){
			int n_lights = scene.get_light_cache().size();
			int light_num = static_cast<int>(l_samples_num[i] * n_lights);
			light_num = std::min(light_num, n_lights - 1);
			//The unordered map isn't a random access container, so 'find' the light_num light
			auto lit = std::find_if(scene.get_light_cache().begin(), scene.get_light_cache().end(),
				[&light_num](const auto&){
					return light_num-- == 0;
				});
			auto &light = *lit->second;
			float pdf_val = 0;
			OcclusionTester occlusion;
			Vector v;
			Colorf light_rad = light.sample(p, LightSample{l_samples_u[i], l_samples_comp[i]}, v, pdf_val, occlusion);
			if (!light_rad.is_black() && pdf_val > 0 && !occlusion.occluded(scene)){
				Colorf light_direct = light_rad * occlusion.transmittance(scene, renderer, sampler, pool);
				Colorf s = transmit * scatter * vol->phase(p, w_o, -v) * light_direct * n_lights / pdf_val;
				rad += s;
			}
		}
	}
	return rad * step;
}
Colorf SingleScatteringIntegrator::transmittance(const Scene &scene, const Renderer&, const RayDifferential &ray,
	Sampler &sampler, MemoryPool&) const
{
	const VolumeNode *vol = scene.get_volume_root();
	if (vol == nullptr){
		return Colorf{1};
	}
	Colorf tau = -vol->optical_thickness(ray, step_size, sampler.random_float());
	return tau.exp();
}


