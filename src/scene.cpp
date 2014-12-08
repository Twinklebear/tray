#include <string>
#include "integrator/volume_integrator.h"
#include "geometry/geometry.h"
#include "volume/volume_node.h"
#include "material/material.h"
#include "lights/light.h"
#include "samplers/sampler.h"
#include "textures/texture.h"
#include "scene.h"

Scene::Scene(Camera camera, RenderTarget target, std::unique_ptr<Sampler> sampler, std::unique_ptr<Renderer> renderer)
	: camera(std::move(camera)), render_target(std::move(target)), sampler(std::move(sampler)),
	renderer(std::move(renderer)), root(nullptr, nullptr, Transform{}, "root"), background(nullptr), environment(nullptr)
{}
GeometryCache& Scene::get_geom_cache(){
	return geom_cache;
}
MaterialCache& Scene::get_mat_cache(){
	return mat_cache;
}
TextureCache& Scene::get_tex_cache(){
	return tex_cache;
}
LightCache& Scene::get_light_cache(){
	return light_cache;
}
const LightCache& Scene::get_light_cache() const {
	return light_cache;
}
VolumeCache& Scene::get_volume_cache(){
	return volume_cache;
}
Camera& Scene::get_camera(){
	return camera;
}
RenderTarget& Scene::get_render_target(){
	return render_target;
}
const RenderTarget& Scene::get_render_target() const {
	return render_target;
}
const Sampler& Scene::get_sampler() const {
	return *sampler;
}
const Renderer& Scene::get_renderer() const {
	return *renderer;
}
Renderer& Scene::get_renderer(){
	return *renderer;
}
Node& Scene::get_root(){
	return root;
}
const Node& Scene::get_root() const {
	return root;
}
VolumeNode* Scene::get_volume_root() {
	return volume_root != nullptr ? volume_root.get() : nullptr;
}
const VolumeNode* Scene::get_volume_root() const {
	return volume_root != nullptr ? volume_root.get() : nullptr;
}
void Scene::set_volume_root(std::unique_ptr<VolumeNode> vol){
	volume_root = std::move(vol);
}
void Scene::set_background(Texture *t){
	background = t;
}
void Scene::set_environment(Texture *t){
	environment = t;
}
const Texture* Scene::get_background() const {
	return background;
}
const Texture* Scene::get_environment() const {
	return environment;
}

