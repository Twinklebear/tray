#include "volume/volume_node.h"

VolumeNode::VolumeNode(const Volume *volume, const Transform &t, const std::string &name)
	: volume(volume), transform(t), inv_transform(t.inverse()), name(name)
{}
BBox VolumeNode::bound() const {
	if (volume){
		return transform(volume->bound());
	}
	return BBox{};
}
bool VolumeNode::intersect(Ray &ray) const {
	bool hit = false;
	if (volume){
		Ray node_space = ray;
		transform(ray, node_space);
		hit = volume->intersect(ray);
	}
	//Note that children are in world space since we apply the transform
	//stack as we load, so use the world space ray here
	for (const auto &c : children){
		hit = c->intersect(ray) || hit;
	}
	return hit;
}
Colorf VolumeNode::absorption(const Point &p, const Vector &v) const {
	if (volume){
		return volume->absorption(p, v);
	}
	return 0;
}
Colorf VolumeNode::scattering(const Point &p, const Vector &v) const {
	if (volume){
		return volume->scattering(p, v);
	}
	return 0;
}
Colorf VolumeNode::attenuation(const Point &p, const Vector &v) const {
	if (volume){
		return volume->attenuation(p, v);
	}
	return 0;
}
Colorf VolumeNode::emission(const Point &p, const Vector &v) const {
	if (volume){
		return volume->emission(p, v);
	}
	return 0;
}
Colorf VolumeNode::optical_thickness(const Ray &ray, float step, float offset) const {
	if (volume){
		return volume->optical_thickness(ray, step, offset);
	}
	return 0;
}
float VolumeNode::phase(const Point &p, const Vector &w_i, const Vector &w_o) const {
	if (volume){
		return volume->phase(p, w_i, w_o);
	}
	return 0;
}
const Volume* VolumeNode::get_volume() const {
	return volume;
}
const Transform& VolumeNode::get_transform() const {
	return transform;
}
Transform& VolumeNode::get_transform(){
	return transform;
}
const Transform& VolumeNode::get_inv_transform() const {
	return inv_transform;
}
Transform& VolumeNode::get_inv_transform(){
	return inv_transform;
}

