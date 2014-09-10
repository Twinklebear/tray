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
		Sampler sampler = queue.get_block();
		if (!sampler.has_samples()){
			break;
		}
		while (sampler.has_samples()){
			std::array<float, 2> s = sampler.get_sample();
			Ray ray = camera.generate_ray(s[0], s[1]);
			Colorf color = shade_ray(ray, scene.get_root());
			color.normalize();
			target.write_pixel(s[0], s[1], color);
			target.write_depth(s[0], s[1], ray.max_t);

			++check_cancel;
			if (check_cancel >= 32){
				check_cancel = 0;
				int canceled = STATUS::CANCELED;
				if (status.compare_exchange_strong(canceled, STATUS::DONE, std::memory_order_acq_rel)){
					return;
				}
			}
		}
	}
	status.store(STATUS::DONE, std::memory_order_release);
}
Colorf Worker::shade_ray(Ray &ray, Node &node){
	Colorf color;
	HitInfo hitinfo;
	if (intersect_nodes(node, ray, hitinfo)){
		const Material *mat = hitinfo.node->get_material();
		if (mat){
			if (mat->is_transparent() && ray.depth < scene.get_max_depth()){
				float r = 0;
				Vector n;
				if (hitinfo.hit_side == HITSIDE::FRONT){
					r = 1.f / mat->refractive_idx();
					n = Vector{hitinfo.normal.normalized()};
				}
				else {
					r = mat->refractive_idx();
					n = -Vector{hitinfo.normal.normalized()};
				}
				float c = -n.dot(ray.d);
				Vector dir = r * ray.d + (r * c - std::sqrt(1 - r * r * (1 - c * c))) * n;
				Ray refr{hitinfo.point, dir.normalized(), ray};
				refr.o += 0.01 * refr.d;
				//TODO: What do we do with refractive and absorption values?
				color += shade_ray(refr, scene.get_root());
			}
			if (mat->is_reflective() && ray.depth < scene.get_max_depth()){
				//Reflect and cast ray
				Vector n{hitinfo.normal.normalized()};
				Vector dir = ray.d - 2 * n.dot(ray.d) * n;
				Ray refl{hitinfo.point, dir.normalized(), ray};
				//Scoot the ray along a bit to prevent self intersection
				refl.o += 0.01 * refl.d;
				color += shade_ray(refl, scene.get_root()) * mat->reflective();
			}
			std::vector<Light*> lights = visible_lights(hitinfo.point);
			color += mat->shade(ray, hitinfo, lights);
		}
		else {
			color = Colorf{0.4, 0.4, 0.4};
		}
	}
	return color;
}
bool Worker::intersect_nodes(Node &node, Ray &ray, HitInfo &hitinfo){
	bool hit = false;
	//Transform the ray into this nodes space
	Ray node_space = ray;
	auto &inv_transform = node.get_inv_transform();
	inv_transform(ray, node_space);
	//Test this node then its children
	if (node.get_geometry() && node.get_geometry()->object_bound().intersect(node_space)){
		hit = node.get_geometry()->intersect(node_space, hitinfo);
		if (hit){
			hitinfo.node = &node;
		}
	}
	for (auto &c : node.get_children()){
		hit = intersect_nodes(*c, node_space, hitinfo) || hit;
	}
	if (hit){
		auto &transform = node.get_transform();
		transform(hitinfo.point, hitinfo.point);
		transform(hitinfo.normal, hitinfo.normal);
		ray.max_t = node_space.max_t;
		if (ray.d.dot(Vector{hitinfo.normal}) <= 0){
			hitinfo.hit_side = HITSIDE::FRONT;
		}
		else {
			hitinfo.hit_side = HITSIDE::BACK;
		}
	}
	return hit;
}
std::vector<Light*> Worker::visible_lights(const Point &p){
	//Maybe the list passed should be a pair of { light, light_mod } to account
	//for absorption when the light is viewed through a transparent object
	std::vector<Light*> lights;
	HitInfo info;
	for (const auto &l : scene.get_light_cache()){
		if (l.second->type() == LIGHT::AMBIENT){
			lights.push_back(l.second.get());
			continue;
		}
		Ray r{p, -l.second->direction(p)};
		//Scoot the ray along the direction a tiny bit to avoid self intersection
		r.o += 0.01 * r.d;
		bool hit = intersect_nodes(scene.get_root(), r, info);
		if (l.second->type() == LIGHT::DIRECT && !hit){
			lights.push_back(l.second.get());
		}
		else if (l.second->type() == LIGHT::POINT){
			//Check that the intersected item isn't behind point lights
			if (!hit || (r(r.max_t) - r.o).length_sqr() > l.second->direction(r.o).length_sqr()){
				lights.push_back(l.second.get());
			}
		}
	}
	return lights;
}

Driver::Driver(Scene &scene, int nworkers, int bwidth, int bheight) : scene(scene),
	queue(Sampler{0, scene.get_render_target().get_width(),
		0, scene.get_render_target().get_height()}, bwidth, bheight)
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


