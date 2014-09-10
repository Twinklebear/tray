#include <string>
#include "geometry/geometry.h"
#include "material/material.h"
#include "lights/light.h"
#include "render/render_target.h"
#include "render/camera.h"
#include "scene.h"

Scene::Scene(const Camera &camera, const RenderTarget &target, int max_depth)
	: camera(camera), render_target(target), root(nullptr, nullptr, Transform{}, "root"),
		max_depth(max_depth)
{}
GeometryCache& Scene::get_geom_cache(){
	return geom_cache;
}
MaterialCache& Scene::get_mat_cache(){
	return mat_cache;
}
LightCache& Scene::get_light_cache(){
	return light_cache;
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
Node& Scene::get_root(){
	return root;
}
int Scene::get_max_depth() const {
	return max_depth;
}

