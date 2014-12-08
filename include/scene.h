#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <string>
#include "geometry/geometry.h"
#include "volume/volume_node.h"
#include "material/material.h"
#include "textures/texture.h"
#include "lights/light.h"
#include "film/render_target.h"
#include "film/camera.h"
#include "samplers/sampler.h"
#include "textures/texture.h"
#include "renderer/renderer.h"
#include "integrator/surface_integrator.h"

/*
 * Describes a scene that we're rendering
 */
class Scene {
	GeometryCache geom_cache;
	MaterialCache mat_cache;
	TextureCache tex_cache;
	LightCache light_cache;
	Camera camera;
	RenderTarget render_target;
	std::unique_ptr<Sampler> sampler;
	std::unique_ptr<Renderer> renderer;
	Node root;
	std::shared_ptr<VolumeNode> volume_root;
	Texture *background, *environment;

public:
	/*
	 * Setup the camera and render target for the scene
	 * Geometry can be added by adding nodes to the root
	 * and selecting from or adding to the geometry cache
	 */
	Scene(Camera camera, RenderTarget target, std::unique_ptr<Sampler> sampler,
		std::unique_ptr<Renderer>);
	GeometryCache& get_geom_cache();
	MaterialCache& get_mat_cache();
	TextureCache& get_tex_cache();
	LightCache& get_light_cache();
	const LightCache& get_light_cache() const;
	Camera& get_camera();
	RenderTarget& get_render_target();
	const RenderTarget& get_render_target() const;
	const Sampler& get_sampler() const;
	const Renderer& get_renderer() const;
	Renderer& get_renderer();
	/*
	 * Get the root node of the geometry scene graph
	 */
	Node& get_root();
	const Node& get_root() const;
	/*
	 * Get the root node of the volume scene graph
	 * Note: this may be null as volumes are optional while
	 * a root geometry node isn't
	 */
	VolumeNode* get_volume_root();
	const VolumeNode* get_volume_root() const;
	void set_background(Texture *t);
	void set_environment(Texture *t);
	const Texture* get_background() const;
	const Texture* get_environment() const;
};

#endif

