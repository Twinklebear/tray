#ifndef DRIVER_H
#define DRIVER_H

#include <vector>
#include <thread>
#include <atomic>
#include "scene.h"
#include "geometry/geometry.h"
#include "linalg/ray.h"
#include "samplers/sampler.h"
#include "block_queue.h"

//Status of a worker thread
enum STATUS { NOT_STARTED, WORKING, DONE, CANCELED, JOINED };

/*
 * A worker thread that renders some subsection of the scene
 */
class Worker {
	Scene &scene;
	BlockQueue &queue;

public:
	//The thread the worker is on
	std::thread thread;
	//The thread status so we can report whether we're done or should cancel
	std::atomic_int status;

	/*
	 * Create the worker to get samplers from the sampler
	 * and use them to render the scene
	 */
	Worker(Scene &scene, BlockQueue &queue);
	Worker(Worker &&w);
	void render();
	/*
	 * Run intersecton tests on a node and its children and report back the results
	 */
	static bool intersect_nodes(Node &node, Ray &ray, HitInfo &hitinfo);
	/*
	 * Get the list of lights visible from some point in the scene
	 * the normal is used to offset the ray start point some so we don't self-intersect
	 */
	std::vector<Light*> visible_lights(const Point &p);
};

/*
 * A driver that distributes the work of rendering the scene
 * among some number of threads
 */
class Driver {
	//The worker threads rendering the scene
	std::vector<Worker> workers;
	Scene &scene;
	//Queue of blocks of pixels to be worked on
	BlockQueue queue;

public:
	/*
	 * Create a driver to render the scene with some number of worker threads
	 * to work on the scene partitioned into blocks with the desired dimensions
	 */
	Driver(Scene &scene, int nworkers, int bwidth, int bheight);
	~Driver();
	void render();
	bool done();
	/*
	 * Abort all threads and cancel rendering
	 */
	void cancel();
	/*
	 * Get the scene being rendered
	 */
	const Scene& get_scene() const;
};

#endif

