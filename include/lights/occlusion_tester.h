#ifndef OCCLUSION_TESTER_H
#define OCCLUSION_TESTER_H

#include "linalg/point.h"
#include "linalg/ray.h"
#include "renderer/renderer.h"
#include "samplers/sampler.h"
#include "memory_pool.h"

class Scene;

/*
 * A structure used to setup and test lights for occlusion
 */
struct OcclusionTester {
	Ray ray;

	/*
	 * Set the occlusion test to be performed between two points
	 */
	void set_points(const Point &a, const Point &b);
	/*
	 * Set the occlusion test to be performed along a ray
	 */
	void set_ray(const Point &p, const Vector &d);
	/*
	 * Test if the ray or segment set is occluded by some object in the scene
	 */
	bool occluded(const Scene &scene);
	/*
	 * Compute the transmittance along the ray/segment that the occlusion test
	 * was performed on
	 */
	Colorf transmittance(const Scene &scene, const Renderer &renderer, Sampler &sampler,
		MemoryPool &pool);
};

#endif

