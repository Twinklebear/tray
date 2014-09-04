#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <string>
#include <memory>
#include "linalg/ray.h"
#include "linalg/transform.h"
#include "material/material.h"
#include "cache.h"
#include "hitinfo.h"

class Geometry {
public:
	/*
	 * Test a ray for intersection with the geometry.
	 * The ray should have been previously transformed into object space
	 * If the hit on this object is nearer than any previous ones seen by
	 * the ray the ray's max_t value will be updated and the hitinfo struct
	 * will be filled with information about the hit. Data stored in
	 * hitinfo will be returned in object space, and the node must be set
	 * outside since the geometry doesn't know which node it's attached too or
	 * its transformation, etc.
	 * If no hit occurs the ray and hitinfo are left unmodified
	 */
	virtual bool intersect(Ray &ray, HitInfo &hitinfo) = 0;
};

typedef Cache<Geometry> GeometryCache;

/*
 * A scene node, based off of Cem's Node in the demo code but with
 * some modifications
 */
class Node {
	std::vector<std::shared_ptr<Node>> children;
	//Non-owning reference to some geometry in the cache
	Geometry *geometry;
	//Non-owning reference to some material in the cache
	Material *material;
	Transform transform, inv_transform;
	std::string name;

public:
	/*
	 * Create a node in the scene graph, placing some named geometry in
	 * the scene
	 */
	Node(Geometry *geom, Material *mat, const Transform &t, const std::string &name);
	const std::vector<std::shared_ptr<Node>>& get_children() const;
	std::vector<std::shared_ptr<Node>>& get_children();
	/*
	 * Get the geometry for the node, or null if it has none associated with it
	 */
	const Geometry* get_geometry() const;
	Geometry* get_geometry();
	/*
	 * Get the material for the node, or null if it has none associated with it
	 */
	const Material* get_material() const;
	Material* get_material();
	/*
	 * Get the transform of the inverse of the node's transform
	 */
	const Transform& get_transform() const;
	Transform& get_transform();
	const Transform& get_inv_transform() const;
	Transform& get_inv_transform();
	const std::string& get_name() const;
};

#endif

