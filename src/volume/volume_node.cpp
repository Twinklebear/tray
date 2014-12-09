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
bool VolumeNode::intersect(const Ray &ray, std::array<float, 2> &t) const {
	bool hit = false;
	if (volume){
		Ray node_space = ray;
		inv_transform(ray, node_space);
		hit = volume->intersect(node_space, t);
	}
	//Note that children are in world space since we apply the transform
	//stack as we load, so use the world space ray here
	for (const auto &c : children){
		hit = c->intersect(ray, t) || hit;
	}
	return hit;
}
Colorf VolumeNode::absorption(const Point &p, const Vector &v) const {
	Colorf absorp{0};
	if (volume){
		absorp += volume->absorption(inv_transform(p), v);
	}
	for (const auto &c : children){
		absorp += c->absorption(p, v);
	}
	return absorp;
}
Colorf VolumeNode::scattering(const Point &p, const Vector &v) const {
	Colorf scatter{0};
	if (volume){
		scatter += volume->scattering(inv_transform(p), v);
	}
	for (const auto &c : children){
		scatter += c->scattering(p, v);
	}
	return scatter;
}
Colorf VolumeNode::attenuation(const Point &p, const Vector &v) const {
	Colorf atten{0};
	if (volume){
		atten += volume->attenuation(inv_transform(p), v);
	}
	for (const auto &c : children){
		atten += c->attenuation(p, v);
	}
	return atten;
}
Colorf VolumeNode::emission(const Point &p, const Vector &v) const {
	Colorf emit{0};
	if (volume){
		emit += volume->emission(inv_transform(p), v);
	}
	for (const auto &c : children){
		emit += c->emission(p, v);
	}
	return emit;
}
Colorf VolumeNode::optical_thickness(const Ray &ray, float step, float offset) const {
	Colorf tau{0};
	if (volume){
		tau += volume->optical_thickness(inv_transform(ray), step, offset);
	}
	for (const auto &c : children){
		tau += c->optical_thickness(ray, step, offset);
	}
	return tau;
}
float VolumeNode::phase(const Point &p, const Vector &w_i, const Vector &w_o) const {
	//Is phase multiplicative or additive? Does it even make sense to combine?
	float phase_val = 0;
	if (volume){
		phase_val += volume->phase(inv_transform(p), inv_transform(w_i), inv_transform(w_o));
	}
	for (const auto &c : children){
		phase_val += c->phase(p, w_i, w_i);
	}
	return phase_val;
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
const std::vector<std::shared_ptr<VolumeNode>>& VolumeNode::get_children() const {
	return children;
}
std::vector<std::shared_ptr<VolumeNode>>& VolumeNode::get_children(){
	return children;
}

