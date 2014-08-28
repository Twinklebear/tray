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
	//Non-owning reference to some geometry in the cache
	Geometry *geometry;
	Transform transform;
	std::string name;

public:
	/*
	 * Create a node in the scene graph, placing some named geometry in
	 * the scene
	 */
	Node(Geometry *geom, const Transform &t, const std::string &name);
	const std::vector<std::shared_ptr<Node>>& get_children() const;
	std::vector<std::shared_ptr<Node>>& get_children();
	/*
	 * Get the geometry for the node, or null if it has none associated with it
	 */
	const Geometry* get_geometry() const;
	Geometry* get_geometry();
	const Transform& get_transform() const;
	Transform& get_transform();
	const std::string& get_name() const;
};

#endif

