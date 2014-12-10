#include <array>
#include <cstring>
#include "scene.h"
#include "material/material.h"
#include "lights/light.h"
#include "lights/area_light.h"
#include "lights/occlusion_tester.h"
#include "material/bsdf.h"
#include "renderer/renderer.h"
#include "integrator/bidir_path_integrator.h"

BidirPathIntegrator::BidirPathIntegrator(int min_depth, int max_depth) : min_depth(min_depth), max_depth(max_depth){}
Colorf BidirPathIntegrator::illumination(const Scene &scene, const Renderer &renderer, const RayDifferential &r,
	DifferentialGeometry&, Sampler &sampler, MemoryPool &pool) const
{
	//TODO: It would be more efficient to trace the camera path of length max_depth - 1 and start from the
	//differential geometry the renderer sends us, instead of re-finding the first hit in trace_path here
	RayDifferential ray = r;
	ray.max_t = std::numeric_limits<float>::infinity();
	auto *cam_path = pool.alloc_array<PathVertex>(max_depth);
	int cam_path_len = trace_path(scene, renderer, ray, Colorf{1}, sampler, pool, cam_path);

	//Now uniformly select a light to sample our light path from
	auto *l_samples_u = pool.alloc_array<std::array<float, 2>>(2);
	auto *l_samples_comp = pool.alloc_array<float>(2);
	sampler.get_samples(l_samples_u, 2);
	sampler.get_samples(l_samples_comp, 2);
	int n_lights = scene.get_light_cache().size();
	if (n_lights == 0){
		return Colorf{0};
	}
	int light_num = static_cast<int>(l_samples_comp[0] * n_lights);
	light_num = std::min(light_num, n_lights - 1);
	//The unordered map isn't a random access container, so 'find' the light_num light
	auto lit = std::find_if(scene.get_light_cache().begin(), scene.get_light_cache().end(),
		[&light_num](const auto&){
			return light_num-- == 0;
		});
	assert(lit != scene.get_light_cache().end());
	const Light &light = *lit->second;
	Ray ray_l;
	Normal n_l;
	float pdf_light = 0;
	Colorf light_weight = light.sample(scene, LightSample{l_samples_u[0], l_samples_comp[1]},
		l_samples_u[1], ray_l, n_l, pdf_light);
	//If the light would have no contribution them just do camera path tracing
	if (light_weight.is_black() || pdf_light == 0){
		return camera_luminance(scene, renderer, cam_path, cam_path_len, sampler, pool);
	}
	//Trace a light path through the scene and then combine it with our camera path to compute
	//the final illumination along the path
	light_weight *= std::abs(ray_l.d.dot(n_l.normalized())) / pdf_light;
	auto *light_path = pool.alloc_array<PathVertex>(max_depth);
	int light_path_len = trace_path(scene, renderer, RayDifferential{ray_l}, light_weight, sampler, pool, light_path);
	return bidir_luminance(scene, renderer, cam_path, cam_path_len, light_path, light_path_len, sampler, pool);
}
int BidirPathIntegrator::trace_path(const Scene &scene, const Renderer &renderer, const RayDifferential &r, const Colorf &weight,
	Sampler &sampler, MemoryPool &pool, PathVertex *path_vertices) const
{
	//This is identical to what we do in the path integrator but we don't do any lighting computation
	//just save information about the vertices we hit to make the path
	auto *path_samples_u = pool.alloc_array<std::array<float, 2>>(max_depth);
	auto *path_samples_comp = pool.alloc_array<float>(max_depth);
	sampler.get_samples(path_samples_u, max_depth);
	sampler.get_samples(path_samples_comp, max_depth);
	RayDifferential ray{r};
	Colorf path_throughput{weight};
	int path_len = 0;
	for (; path_len < max_depth; ++path_len){
		PathVertex &v = path_vertices[path_len];
		if (!scene.get_root().intersect(ray, v.dg) || !v.dg.node->get_material()){
			break;
		}
		v.throughput = path_throughput;
		v.dg.compute_differentials(ray);
		BSDF *bsdf = v.dg.node->get_material()->get_bsdf(v.dg, pool);
		v.bsdf = bsdf;
		v.w_o = -ray.d;
		float pdf_val = 0;
		BxDFTYPE sampled_type;
		Colorf f = bsdf->sample(v.w_o, v.w_i, path_samples_u[path_len], path_samples_comp[path_len],
			pdf_val, BxDFTYPE::ALL, &sampled_type);
		v.specular_bounce = (sampled_type & BxDFTYPE::SPECULAR) != 0;
		v.num_specular_comp = bsdf->num_bxdfs(BxDFTYPE(BxDFTYPE::SPECULAR | BxDFTYPE::REFLECTION | BxDFTYPE::TRANSMISSION));

		//If there's no further illumination that will come from this vertex we can stop
		if (f.is_black() || pdf_val == 0){
			return path_len + 1;
		}
		//Check if we should terminate the path using Russian roulette after reaching min_depth
		Colorf survival_weight = f * std::abs(v.w_i.dot(v.bsdf->dg.normal)) / pdf_val;
		path_throughput *= survival_weight;
		if (path_len > min_depth){
			float cont_prob = std::min(1.f, survival_weight.luminance());
			if (sampler.random_float() > cont_prob){
				return path_len + 1;
			}
			path_throughput /= cont_prob;
		}
		path_throughput *= renderer.transmittance(scene, ray, sampler, pool);
		ray = RayDifferential{v.bsdf->dg.point, v.w_i, ray, 0.001};
	}
	return path_len;
}
Colorf BidirPathIntegrator::camera_luminance(const Scene &scene, const Renderer &renderer, const PathVertex *path_vertices,
	int path_len, Sampler &sampler, MemoryPool &pool) const
{
	auto *l_samples_u = pool.alloc_array<std::array<float, 2>>(path_len);
	auto *l_samples_comp = pool.alloc_array<float>(path_len);
	auto *bsdf_samples_u = pool.alloc_array<std::array<float, 2>>(path_len);
	auto *bsdf_samples_comp = pool.alloc_array<float>(path_len);
	sampler.get_samples(l_samples_u, path_len);
	sampler.get_samples(l_samples_comp, path_len);
	sampler.get_samples(bsdf_samples_u, path_len);
	sampler.get_samples(bsdf_samples_comp, path_len);
	Colorf illum;
	//We need to track some information about whether we came from a specular bounce to determine
	//how to handle lighting computations
	bool prev_specular = true;
	for (int i = 0; i < path_len; ++i){
		const PathVertex &v_c = path_vertices[i];
		const Point &p_c = v_c.bsdf->dg.point;
		const Normal &n_c = v_c.bsdf->dg.normal;
		//Handle direct lighting when hitting lights as done in path tracing
		if (prev_specular && v_c.dg.node->get_area_light()){
			illum += v_c.throughput * v_c.dg.node->get_area_light()->radiance(p_c, n_c, v_c.w_o);
		}
		//Uniformly sample one of the lights contribution to the point if we're not estimating it through
		//specular bounces for all vertices
		illum += v_c.throughput * uniform_sample_one_light(scene, renderer, p_c, n_c, v_c.w_o, *v_c.bsdf,
			LightSample{l_samples_u[i], l_samples_comp[i]},
			BSDFSample{bsdf_samples_u[i], bsdf_samples_comp[i]}, sampler, pool);
		prev_specular = v_c.specular_bounce;
	}
	return illum;
}
Colorf BidirPathIntegrator::bidir_luminance(const Scene &scene, const Renderer &renderer, PathVertex *cam_path,
	int cam_path_len, PathVertex *light_path, int light_path_len, Sampler &sampler, MemoryPool &pool) const
{
	auto *l_samples_u = pool.alloc_array<std::array<float, 2>>(cam_path_len);
	auto *l_samples_comp = pool.alloc_array<float>(cam_path_len);
	auto *bsdf_samples_u = pool.alloc_array<std::array<float, 2>>(cam_path_len);
	auto *bsdf_samples_comp = pool.alloc_array<float>(cam_path_len);
	sampler.get_samples(l_samples_u, cam_path_len);
	sampler.get_samples(l_samples_comp, cam_path_len);
	sampler.get_samples(bsdf_samples_u, cam_path_len);
	sampler.get_samples(bsdf_samples_comp, cam_path_len);
	//The number of specular vertices encountered along each path also effects the total number of
	//possible paths that could be traced to compute the light and we must account for this reduction
	//in probability
	const int num_verts = cam_path_len + light_path_len + 2;
	auto *num_spec_verts = pool.alloc_array<int>(num_verts);
	std::memset(num_spec_verts, 0, num_verts * sizeof(int));
	for (int i = 0; i < cam_path_len; ++i){
		for (int j = 0; j < light_path_len; ++j){
			if (cam_path[i].specular_bounce || light_path[j].specular_bounce){
				++num_spec_verts[i + j + 2];
			}
		}
	}
	Colorf illum;
	//We need to track some information about whether we came from a specular bounce to determine
	//how to handle lighting computations
	bool prev_specular = true;
	//Go through each vertex on the camera path and combine it with each visible vertex on the light
	//path, also taking into account direct illumination effects as done in trace_camera
	for (int i = 0; i < cam_path_len; ++i){
		const PathVertex &v_c = cam_path[i];
		const Point &p_c = v_c.bsdf->dg.point;
		const Normal &n_c = v_c.bsdf->dg.normal;
		//Handle direct lighting when hitting lights as done in path tracing
		if (prev_specular && v_c.dg.node->get_area_light()){
			illum += v_c.throughput * v_c.dg.node->get_area_light()->radiance(p_c, n_c, v_c.w_o);
		}
		//Uniformly sample one of the lights contribution to the point if we're not estimating it through
		//specular bounces for all vertices
		Colorf direct = v_c.throughput * uniform_sample_one_light(scene, renderer, p_c, n_c, v_c.w_o, *v_c.bsdf,
			LightSample{l_samples_u[i], l_samples_comp[i]},
			BSDFSample{bsdf_samples_u[i], bsdf_samples_comp[i]}, sampler, pool);
		prev_specular = v_c.specular_bounce;
		//Account for the number of other possible paths that could compute this contribution for the direct light
		illum += direct / (i + 1 - num_spec_verts[i + 1]);
		//If there's possible contribution from the light paths (eg. we aren't a specular bounce) compute it
		if (!v_c.specular_bounce){
			for (int j = 0; j < light_path_len; ++j){
				const PathVertex &v_l = light_path[j];
				const Point &p_l = v_l.bsdf->dg.point;
				const Normal &n_l = v_l.bsdf->dg.normal;
				if (!v_l.specular_bounce){
					Vector w = (p_l - p_c).normalized();
					Colorf f_c = (*v_c.bsdf)(v_c.w_o, w) * (1 + v_c.num_specular_comp);
					Colorf f_l = (*v_l.bsdf)(-w, v_l.w_o) * (1 + v_l.num_specular_comp);
					if (f_c.is_black() || f_l.is_black()){
						continue;
					}
					//Visibility test for the vertices on the camera and light path we're trying to connect
					Ray vis{p_c, p_l - p_c, 0.001, 0.999};
					DifferentialGeometry dg;
					if (!scene.get_root().intersect(vis, dg)){
						//TODO: multiple importance sampling?
						float weight = 1.f / (i + j + 2 - num_spec_verts[i + j + 2]);
						float geom_term = std::abs(w.dot(n_c)) * std::abs(w.dot(n_l)) / p_l.distance_sqr(p_c);
						//Multiply by transmittance here
						illum += v_c.throughput * f_c * geom_term * f_l * v_l.throughput * weight
							* renderer.transmittance(scene, RayDifferential{vis}, sampler, pool);
					}
				}
			}
		}
	}
	return illum;
}

