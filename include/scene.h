#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <string>
#include "geometry/geometry.h"
#include "material/pbr_material.h"
#include "textures/texture.h"
#include "lights/light.h"
#include "film/render_target.h"
#include "film/camera.h"
#include "samplers/sampler.h"
#include "textures/texture.h"

/*
 * Describes a scene that we're rendering
 */
class Scene {
	GeometryCache geom_cache;
	PBRMaterialCache mat_cache;
	TextureCache tex_cache;
	LightCache light_cache;
	Camera camera;
	RenderTarget render_target;
	std::unique_ptr<Sampler> sampler;
	Node root;
	//The max recursion depth for reflected/refracted rays
	int max_depth;
	Texture *background, *environment;

public:
	/*
	 * Setup the camera and render target for the scene
	 * Geometry can be added by adding nodes to the root
	 * and selecting from or adding to the geometry cache
	 */
	Scene(Camera camera, RenderTarget target, std::unique_ptr<Sampler> sampler, int depth);
	GeometryCache& get_geom_cache();
	PBRMaterialCache& get_mat_cache();
	TextureCache& get_tex_cache();
	LightCache& get_light_cache();
	const LightCache& get_light_cache() const;
	Camera& get_camera();
	RenderTarget& get_render_target();
	const RenderTarget& get_render_target() const;
	const Sampler& get_sampler() const;
	Node& get_root();
	const Node& get_root() const;
	int get_max_depth() const;
	void set_background(Texture *t);
	void set_environment(Texture *t);
	const Texture* get_background() const;
	const Texture* get_environment() const;
};

#endif

