#include <vector>
#include <algorithm>
#include <thread>
#include <atomic>
#include "scene.h"
#include "samplers/sampler.h"
#include "film/render_target.h"
#include "film/camera.h"
#include "renderer/renderer.h"
#include "integrator/path_integrator.h"
#include "integrator/bidir_path_integrator.h"
#include "geometry/geometry.h"
#include "linalg/ray.h"
#include "linalg/transform.h"
#include "memory_pool.h"
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
	const Renderer &renderer = scene.get_renderer();
	MemoryPool pool;
	std::vector<Sample> samples;
	std::vector<RayDifferential> rays;
	std::vector<Colorf> colors;
	//Counter so we can check if we've been canceled, check after every 32 pixels rendered
	int check_cancel = 0;
	while (true){
		Sampler *sampler = queue.get_block();
		if (!sampler){
			break;
		}
		samples.resize(sampler->get_max_spp());
		rays.reserve(sampler->get_max_spp());
		colors.reserve(sampler->get_max_spp());
		while (sampler->has_samples()){
			sampler->get_samples(samples);
			for (const auto &s : samples){
				rays.push_back(camera.generate_raydifferential(s));
				rays.back().scale_differentials(1.f / std::sqrt(sampler->get_max_spp()));
				colors.push_back(renderer.illumination(rays.back(), scene, *sampler, pool));
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
				pool.free_blocks();
			}
			if (sampler->report_results(samples, rays, colors)){
				for (size_t i = 0; i < samples.size(); ++i){
					target.write_pixel(samples[i].img[0], samples[i].img[1], colors[i]);
					target.write_float(samples[i].img[0], samples[i].img[1], samples.size());
				}
				rays.clear();
				colors.clear();
			}
		}
	}
	status.store(STATUS::DONE, std::memory_order_release);
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
	scene.get_renderer().preprocess(scene);
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

