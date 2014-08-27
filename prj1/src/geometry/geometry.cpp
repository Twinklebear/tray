#include <vector>
#include <string>
#include <memory>
#include "linalg/ray.h"
#include "linalg/transform.h"
#include "geometry/geometry.h"

Node::Node(const std::shared_ptr<Geometry> &geom, const Transform &t,
	const std::string &name)
	: geometry(geom), transform(t), name(name)
{}
const std::vector<std::shared_ptr<Node>>& Node::get_children() const {
	return children;
}
std::vector<std::shared_ptr<Node>>& Node::get_children(){
	return children;
}
const Geometry& Node::get_geometry() const {
	return *geometry;
}
const Transform& Node::get_transform() const {
	return transform;
}
Transform& Node::get_transform(){
	return transform;
}

