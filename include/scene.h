#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <string>
#include "geometry/geometry.h"
#include "material/material.h"
#include "lights/light.h"
#include "render/render_target.h"
#include "render/camera.h"
#include "samplers/sampler.h"

/*
 * Describes a scene that we're rendering
 */
class Scene {
	GeometryCache geom_cache;
	MaterialCache mat_cache;
	LightCache light_cache;
	Camera camera;
	RenderTarget render_target;
	std::unique_ptr<Sampler> sampler;
	Node root;
	//The max recursion depth for reflected/refracted rays
	int max_depth;

public:
	/*
	 * Setup the camera and render target for the scene
	 * Geometry can be added by adding nodes to the root
	 * and selecting from or adding to the geometry cache
	 */
	Scene(Camera camera, RenderTarget target, std::unique_ptr<Sampler> sampler,
		int depth);
	GeometryCache& get_geom_cache();
	MaterialCache& get_mat_cache();
	LightCache& get_light_cache();
	Camera& get_camera();
	RenderTarget& get_render_target();
	const RenderTarget& get_render_target() const;
	const Sampler& get_sampler() const;
	Node& get_root();
	int get_max_depth() const;
};

#endif

