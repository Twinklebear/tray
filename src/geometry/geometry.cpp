#include <cassert>
#include <memory>
#include <algorithm>
#include <vector>
#include <string>
#include <memory>
#include "lights/area_light.h"
#include "linalg/ray.h"
#include "linalg/transform.h"
#include "accelerators/bvh.h"
#include "geometry/differential_geometry.h"
#include "geometry/geometry.h"

float Geometry::surface_area() const {
	assert("Unimplemented surface area called");
	return 0;
}
Point Geometry::sample(const GeomSample&, Normal&) const {
	assert("Unimplemented sample called");
	return Point{0};
}
Point Geometry::sample(const Point&, const GeomSample &gs, Normal &normal) const {
	return sample(gs, normal);
}
float Geometry::pdf(const Point&) const {
	return 1.f / surface_area();
}
float Geometry::pdf(const Point &p, const Vector &w_i) const {
	DifferentialGeometry dg;
	Ray ray{p, w_i, 0.001};
	if (!intersect(ray, dg)){
		return 0;
	}
	//Convert PDF over area to be over solid angle
	float pdf_val = p.distance_sqr(ray(ray.max_t)) / (std::abs(dg.normal.dot(-w_i)) * surface_area());
	return std::isinf(pdf_val) ? 0 : pdf_val;
}
bool Geometry::attach_light(const Transform&){
	return false;
}

Node::Node(Geometry *geom, Material *mat, const Transform &t, const std::string &name)
	: geometry(geom), material(mat), transform(t), inv_transform(t.inverse()), name(name), area_light(nullptr)
{}
void Node::attach_light(AreaLight *light){
	area_light = light;
	if (geometry && !geometry->attach_light(transform)){
		assert("Invalid light attachment");
	}
}
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
const AreaLight* Node::get_area_light() const {
	return area_light;
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

