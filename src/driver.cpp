#include <vector>
#include <algorithm>
#include <thread>
#include <atomic>
#include "scene.h"
#include "samplers/sampler.h"
#include "render/render_target.h"
#include "render/camera.h"
#include "geometry/geometry.h"
#include "linalg/ray.h"
#include "linalg/transform.h"
#include "driver.h"

Worker::Worker(Scene &scene, BlockQueue &queue)
	: scene(scene), queue(queue), status(STATUS::NOT_STARTED)
{}
Worker::Worker(Worker &&w) : scene(w.scene), queue(w.queue),
	thread(std::move(w.thread)), status(w.status.load(std::memory_order_acquire))
{}
void Worker::render(){
	status.store(STATUS::WORKING, std::memory_order_release);
	Node &root = scene.get_root();
	RenderTarget &target = scene.get_render_target();
	Camera &camera = scene.get_camera();
	//Counter so we can check if we've been canceled, check after every 32 pixels rendered
	int check_cancel = 0;
	while (true){
		Sampler *sampler = queue.get_block();
		if (!sampler){
			break;
		}
		std::vector<Sample> samples;
		std::vector<RayDifferential> rays;
		std::vector<Colorf> colors;
		while (sampler->has_samples()){
			sampler->get_samples(samples);
			rays.reserve(samples.size());
			colors.reserve(samples.size());
			for (const auto &s : samples){
				rays.push_back(camera.generate_raydifferential(s));
				rays.back().scale_differentials(1.f / std::sqrt(sampler->get_max_spp()));
				colors.push_back(shade_ray(rays.back(), scene.get_root()));
				//If we didn't hit anything and the scene has a background use that
				if (scene.get_background() && rays.back().max_t == std::numeric_limits<float>::infinity()){
					DifferentialGeometry dg;
					dg.u = s.img[0] / target.get_width();
					dg.v = s.img[1] / target.get_height();
					colors.back() = scene.get_background()->sample(dg);
				}
				colors.back().normalize();

				++check_cancel;
				if (check_cancel >= 32){
					check_cancel = 0;
					int canceled = STATUS::CANCELED;
					if (status.compare_exchange_strong(canceled, STATUS::DONE, std::memory_order_acq_rel)){
						return;
					}
				}
			}
			if (sampler->report_results(samples, rays, colors)){
				for (size_t i = 0; i < samples.size(); ++i){
					target.write_pixel(samples[i].img[0], samples[i].img[1], colors[i]);
					target.write_float(samples[i].img[0], samples[i].img[1], samples.size());
				}
			}
			rays.clear();
			colors.clear();
		}
	}
	status.store(STATUS::DONE, std::memory_order_release);
}
Colorf Worker::shade_ray(RayDifferential &ray, Node &node){
	Colorf color;
	DifferentialGeometry diff_geom;
	if (intersect_nodes(node, ray, diff_geom)){
		const Material *mat = diff_geom.node->get_material();
		if (mat){
			diff_geom.compute_differentials(ray);
			if (ray.depth < scene.get_max_depth()){
				//Track reflection contribution from Fresnel term to be incorporated
				//into reflection calculation
				Colorf fresnel_refl;
				if (mat->is_transparent()){
					float eta = 0;
					Vector n;
					//Compute proper refractive index ratio and set normal to be on same side
					//as indicident ray for refraction computation when entering/exiting material
					if (diff_geom.hit_side == HITSIDE::FRONT){
						eta = 1.f / mat->refractive_idx();
						n = Vector{diff_geom.normal.normalized()};
					}
					else {
						eta = mat->refractive_idx();
						n = -Vector{diff_geom.normal.normalized()};
					}
					//Compute Schlick's approximation to find amount reflected and transmitted at the surface
					//Note that we use -ray.d here since V should be from point -> camera and we use
					//refl_dir as the "light" direction since that's the light reflection we're interested in
					Vector refl_dir = ray.d - 2 * n.dot(ray.d) * n;
					Vector h = (refl_dir - ray.d).normalized();
					float r = std::pow((mat->refractive_idx() - 1) / (mat->refractive_idx() + 1), 2.f);
					r = r + (1 - r) * std::pow(1 - h.dot(-ray.d), 5);

					//Compute the contribution from light refracting through the object and check for total
					//internal reflection
					float c = -n.dot(ray.d);
					float root = 1 - eta * eta * (1 - c * c);
					if (root > 0){
						RayDifferential refr = ray.refract(diff_geom, n, eta);
						//Account for absorption by the object if the refraction ray we're casting is entering it
						Colorf refr_col = shade_ray(refr, scene.get_root()) * mat->refractive(diff_geom) * (1 - r);
						if (diff_geom.hit_side == HITSIDE::FRONT){
							Colorf absorbed = mat->absorbed(diff_geom);
							color += refr_col * Colorf{std::exp(-refr.max_t * absorbed.r),
								std::exp(-refr.max_t * absorbed.g), std::exp(-refr.max_t * absorbed.b)};
						}
						else {
							color += refr_col;
						}
					}
					//In the case of total internal reflection all the contribution is from the reflected term
					else {
						r = 1;
					}
					//Add Fresnel reflection contribution to be used when computing reflection
					fresnel_refl = mat->refractive(diff_geom) * r;
				}
				if (mat->is_reflective() || fresnel_refl != Colorf{0, 0, 0}){
					Colorf refl_col = mat->reflective(diff_geom) + fresnel_refl;
					//Reflect and cast ray
					RayDifferential refl = ray.reflect(diff_geom);
					color += shade_ray(refl, scene.get_root()) * refl_col;
				}
			}
			std::vector<Light*> lights = visible_lights(diff_geom.point, diff_geom.normal);
			color += mat->shade(ray, diff_geom, lights);
		}
		else {
			color = Colorf{0.4, 0.4, 0.4};
		}
	}
	else if (scene.get_environment()){
		DifferentialGeometry dg;
		dg.point = Point{ray.d.x, ray.d.y, ray.d.z};
		color = scene.get_environment()->sample(dg);
	}
	return color;
}
bool Worker::intersect_nodes(Node &node, Ray &ray, DifferentialGeometry &diff_geom){
	if (node.bound().intersect(ray)){
		return node.intersect(ray, diff_geom);
	}
	return false;
}
std::vector<Light*> Worker::visible_lights(const Point &p, const Normal &n){
	//Maybe the list passed should be a pair of { light, light_mod } to account
	//for absorption when the light is viewed through a transparent object
	std::vector<Light*> lights;
	DifferentialGeometry diff_geom;
	for (const auto &l : scene.get_light_cache()){
		if (l.second->type() == LIGHT::AMBIENT){
			lights.push_back(l.second.get());
			continue;
		}
		//Check that this light would even make a contribution to the surface
		if (n.dot(-l.second->direction(p)) <= 0.f){
			continue;
		}
		Ray r{p, -l.second->direction(p), 0.001};
		if (l.second->type() == LIGHT::DIRECT && !intersect_nodes(scene.get_root(), r, diff_geom)){
			lights.push_back(l.second.get());
		}
		else if (l.second->type() == LIGHT::POINT){
			//Set max_t as well to not get intersections past the point light
			r.max_t = 1;
			if (!intersect_nodes(scene.get_root(), r, diff_geom)){
				lights.push_back(l.second.get());
			}
		}
	}
	return lights;
}

Driver::Driver(Scene &scene, int nworkers, int bwidth, int bheight)
	: scene(scene), queue(scene.get_sampler(), bwidth, bheight)
{
	for (int i = 0; i < nworkers; ++i){
		workers.emplace_back(Worker{scene, queue});
	}
}
Driver::~Driver(){
	//Tell all the threads to cancel
	cancel();
}
void Driver::render(){
	//Run through and launch each thread
	for (auto &w : workers){
		w.thread = std::thread(&Worker::render, std::ref(w));
	}
}
bool Driver::done(){
	//Check which workers have finished and join them, if all are done
	//report that we're done
	bool all_done = true;
	for (auto &w : workers){
		int status = w.status.load(std::memory_order_acquire);
		if (status == STATUS::DONE){
			w.thread.join();
			w.status.store(STATUS::JOINED, std::memory_order_release);
		}
		else if (status != STATUS::JOINED){
			all_done = false;
		}
	}
	return all_done;
}
void Driver::cancel(){
	//Inform all the threads they should quit
	for (auto &w : workers){
		int status = STATUS::WORKING;
		if (w.status.compare_exchange_strong(status, STATUS::CANCELED, std::memory_order_acq_rel)){
			w.thread.join();
			w.status.store(STATUS::JOINED, std::memory_order_release);
		}
		else if (status == STATUS::DONE){
			w.thread.join();
			w.status.store(STATUS::JOINED, std::memory_order_release);
		}
	}
}
const Scene& Driver::get_scene() const {
	return scene;
}

