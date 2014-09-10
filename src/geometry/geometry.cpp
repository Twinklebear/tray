#include <vector>
#include <string>
#include "linalg/ray.h"
#include "linalg/transform.h"
#include "geometry/geometry.h"

Node::Node(Geometry *geom, Material *mat, const Transform &t, const std::string &name)
	: geometry(geom), material(mat), transform(t), inv_transform(t.inverse()), name(name)
{}
const std::vector<std::shared_ptr<Node>>& Node::get_children() const {
	return children;
}
std::vector<std::shared_ptr<Node>>& Node::get_children(){
	return children;
}
const Geometry* Node::get_geometry() const {
	return geometry;
}
Geometry* Node::get_geometry(){
	return geometry;
}
const Material* Node::get_material() const {
	return material;
}
Material* Node::get_material(){
	return material;
}
const Transform& Node::get_transform() const {
	return transform;
}
Transform& Node::get_transform(){
	return transform;
}
const Transform& Node::get_inv_transform() const {
	return inv_transform;
}
Transform& Node::get_inv_transform(){
	return inv_transform;
}
const std::string& Node::get_name() const {
	return name;
}
BBox Node::world_bound() const {
	if (geometry){
		return transform(geometry->object_bound());
	}
	return BBox{};
}

