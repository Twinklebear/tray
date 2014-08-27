#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <string>
#include <memory>
#include "linalg/ray.h"
#include "linalg/transform.h"

class Geometry {
public:
	/*
	 * Test a ray for intersection with the geometry.
	 * The ray should have been previously transformed into object space
	 */
	virtual bool intersect(Ray &r) = 0;
};

class Node {
	std::vector<std::shared_ptr<Node>> children;
	std::shared_ptr<Geometry> geometry;
	Transform transform;
	std::string name;

public:
	/*
	 * Create a node in the scene graph, placing some named geometry in
	 * the scene
	 */
	Node(const std::shared_ptr<Geometry> &geom, const Transform &t,
		const std::string &name);
	const std::vector<std::shared_ptr<Node>>& get_children() const;
	std::vector<std::shared_ptr<Node>>& get_children();
	const Geometry& get_geometry() const;
	const Transform& get_transform() const;
	Transform& get_transform();
};

#endif

