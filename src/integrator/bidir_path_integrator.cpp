#include <array>
#include "material/material.h"
#include "lights/light.h"
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
	auto *cam_path_verts = pool.alloc_array<PathVertex>(max_depth);
	int cam_path_len = trace_path(scene, ray, Colorf{1}, sampler, pool, cam_path_verts);
	//Compute with path tracing for sanity check temporarily
	return camera_luminance(scene, renderer, cam_path_verts, cam_path_len, sampler, pool);
}
int BidirPathIntegrator::trace_path(const Scene &scene, const RayDifferential &r, const Colorf &weight,
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
			BSDFSample{bsdf_samples_u[i], bsdf_samples_comp[i]});
		prev_specular = v_c.specular_bounce;
	}
	return illum;
}

