#include <vector>
#include <string>
#include "linalg/ray.h"
#include "linalg/transform.h"
#include "geometry/geometry.h"

Node::Node(Geometry *geom, const Transform &t, const std::string &name)
	: geometry(geom), transform(t), name(name)
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
const Transform& Node::get_transform() const {
	return transform;
}
Transform& Node::get_transform(){
	return transform;
}
const std::string& Node::get_name() const {
	return name;
}

