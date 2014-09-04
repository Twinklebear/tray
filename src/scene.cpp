#include <string>
#include "geometry/geometry.h"
#include "render/render_target.h"
#include "render/camera.h"
#include "scene.h"

Scene::Scene(const Camera &camera, const RenderTarget &target)
	: camera(camera), render_target(target), root(nullptr, nullptr, Transform{}, "root")
{}
GeometryCache& Scene::get_geom_cache(){
	return geom_cache;
}
MaterialCache& Scene::get_mat_cache(){
	return mat_cache;
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

