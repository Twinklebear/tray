#ifndef SCENE_H
#define SCENE_H

#include <string>
#include "geometry/geometry.h"
#include "geometry/geometry_cache.h"
#include "render/render_target.h"
#include "render/camera.h"

/*
 * Describes a scene that we're rendering
 */
class Scene {
	GeometryCache geom_cache;
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
	Camera& get_camera();
	RenderTarget& get_render_target();
	Node& get_root();
};

#endif

