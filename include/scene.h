#ifndef SCENE_H
#define SCENE_H

#include <string>
#include "geometry/geometry.h"
#include "material/material.h"
#include "lights/light.h"
#include "render/render_target.h"
#include "render/camera.h"

/*
 * Describes a scene that we're rendering
 */
class Scene {
	GeometryCache geom_cache;
	MaterialCache mat_cache;
	LightCache light_cache;
	Camera camera;
	RenderTarget render_target;
	Node root;

public:
	/*
	 * Setup the camera and render target for the scene
	 * Geometry can be added by adding nodes to the root
	 * and selecting from or adding to the geometry cache
	 */
	Scene(const Camera &camera, const RenderTarget &target);
	GeometryCache& get_geom_cache();
	MaterialCache& get_mat_cache();
	LightCache& get_light_cache();
	Camera& get_camera();
	RenderTarget& get_render_target();
	const RenderTarget& get_render_target() const;
	Node& get_root();
};

#endif

