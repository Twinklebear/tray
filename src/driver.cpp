#include <vector>
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
			HitInfo hitinfo;
			if (intersect_nodes(root, ray, hitinfo)){
				Colorf color;
				const Material *mat = hitinfo.node->get_material();
				if (mat){
					color = mat->shade(ray, hitinfo, scene.get_light_cache());
				}
				else {
					color = Colorf{0.4, 0.4, 0.4};
				}
				color.normalize();
				target.write_pixel(s[0], s[1], color);
				target.write_depth(s[0], s[1], ray.max_t);
			}
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
bool Worker::intersect_nodes(Node &node, Ray &ray, HitInfo &hitinfo){
	bool hit = false;
	//Transform the ray into this nodes space
	Ray node_space = ray;
	auto &inv_transform = node.get_inv_transform();
	inv_transform(ray, node_space);
	//Test this node then its children
	if (node.get_geometry()){
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
	}
	return hit;
}
Driver::Driver(Scene &scene, int nworkers, int blocks) : scene(scene),
	queue(Sampler{0, scene.get_render_target().get_width(),
		0, scene.get_render_target().get_height()}, blocks)
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


