#include <algorithm>
#include <vector>
#include <string>
#include <memory>
#include "linalg/ray.h"
#include "linalg/transform.h"
#include "accelerators/bvh.h"
#include "geometry/differential_geometry.h"
#include "geometry/geometry.h"

Node::Node(Geometry *geom, Material *mat, const Transform &t, const std::string &name)
	: geometry(geom), material(mat), transform(t), inv_transform(t.inverse()), name(name)
{}
void Node::flatten_children(){
	std::vector<std::shared_ptr<Node>> flat_children;
	for (auto &c : children){
		c->transform = transform * c->transform;
		c->inv_transform = c->inv_transform * inv_transform;
		if (c->geometry){
			flat_children.push_back(c);
		}
		c->flatten_children(flat_children);
	}
	children = std::move(flat_children);
	std::vector<Geometry*> prims;
	refine(prims);
	bvh = std::make_unique<BVH>(prims, SPLIT_METHOD::SAH, 8);
}
bool Node::intersect(Ray &ray, DifferentialGeometry &diff_geom) const {
	if (bvh){
		return bvh->intersect(ray, diff_geom);
	}
	bool hit = false;
	Ray node_space = ray;
	inv_transform(ray, node_space);
	if (geometry){
		hit = geometry->intersect(node_space, diff_geom);
		if (hit){
			diff_geom.node = this;
		}
	}
	for (auto &c : children){
		hit = c->intersect(node_space, diff_geom) || hit;
	}
	if (hit){
		transform(diff_geom, diff_geom);
		ray.max_t = node_space.max_t;
	}
	return hit;
}
BBox Node::bound() const {
	if (bvh){
		return bvh->bounds();
	}
	if (geometry){
		return transform(geometry->bound());
	}
	return BBox{};
}
void Node::refine(std::vector<Geometry*> &prims){
	if (geometry){
		prims.push_back(this);
	}
	for (auto &c : children){
		if (c->geometry){
			prims.push_back(c.get());
		}
	}
}
const std::vector<std::shared_ptr<Node>>& Node::get_children() const {
	return children;
}
std::vector<std::shared_ptr<Node>>& Node::get_children(){
	return children;
}
const Geometry* Node::get_geometry() const {
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
void Node::flatten_children(std::vector<std::shared_ptr<Node>> &nodes){
	for (auto &c : children){
		c->transform = transform * c->transform;
		c->inv_transform = c->inv_transform * inv_transform;
		if (c->geometry){
			nodes.push_back(c);
		}
		c->flatten_children(nodes);
	}
	children.clear();
}

