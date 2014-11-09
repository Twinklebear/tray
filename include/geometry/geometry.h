#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <string>
#include <memory>
#include "linalg/ray.h"
#include "linalg/transform.h"
#include "material/material.h"
#include "accelerators/bvh.h"
#include "cache.h"
#include "differential_geometry.h"
#include "lights/area_light.h"
#include "bbox.h"

class Geometry {
public:
	/*
	 * Test a ray for intersection with the geometry.
	 * The ray should have been previously transformed into object space
	 * If the hit on this object is nearer than any previous ones seen by
	 * the ray the ray's max_t value will be updated and the differential
	 * geometry struct will be filled with information about the hit.
	 * Data stored in diff_geom will be returned in object space
	 * If no hit occurs the ray and hitinfo are left unmodified
	 */
	virtual bool intersect(Ray &ray, DifferentialGeometry &diff_geom) const = 0;
	/*
	 * Get the object-space AABB for the object
	 */
	virtual BBox bound() const = 0;
	/*
	 * Request that the primitive fully refine itself into
	 * its component geometric primitives and fill prims with them
	 */
	virtual void refine(std::vector<Geometry*> &prims) = 0;
};

typedef Cache<Geometry> GeometryCache;

/*
 * A scene node, based off of Cem's Node in the demo code but with
 * some modifications
 */
class Node : public Geometry {
	std::vector<std::shared_ptr<Node>> children;
	//Non-owning reference to some geometry in the cache
	Geometry *geometry;
	//Non-owning reference to some material in the cache
	Material *material;
	Transform transform, inv_transform;
	std::string name;
	AreaLight *area_light;
	//BVH is created for the root node of the scene only
	std::unique_ptr<BVH> bvh;

public:
	/*
	 * Create a node in the scene graph, placing some named geometry in the scene
	 */
	Node(Geometry *geom, Material *mat, const Transform &t, const std::string &name);
	/*
	 * Attach an area light to the geometry with some intensity
	 */
	void attach_light(AreaLight *light);
	/*
	 * Instruct the node to flatten its children into a vector and build a BVH for them
	 * to accelerate intersection tests. Child transforms will also be brought up into
	 * world space so that the BVH can be built
	 */
	void flatten_children();
	/*
	 * Test the ray for intersection agains this node and its children
	 */
	bool intersect(Ray &ray, DifferentialGeometry &diff_geom) const override;
	/*
	 * Get the world space bound for the object
	 * Returns degenerate box if the node doesn't have geometry attached
	 */
	BBox bound() const override;
	/*
	 * Request that the primitive fully refine itself into
	 * its component geometric primitives and fill prims with them
	 */
	void refine(std::vector<Geometry*> &prims) override;
	/*
	 * Get the node's children
	 * Note: after flattening all the children positions will be in world space
	 */
	const std::vector<std::shared_ptr<Node>>& get_children() const;
	std::vector<std::shared_ptr<Node>>& get_children();
	/*
	 * Get the geometry for the node, or null if it has none associated with it
	 */
	const Geometry* get_geometry() const;
	/*
	 * Get the material for the node, or null if it has none associated with it
	 */
	const Material* get_material() const;
	Material* get_material();
	/*
	 * Get the area light attached to the geometry, returns nullptr if none is attached
	 */
	const AreaLight* get_area_light() const;
	/*
	 * Get the transform of the inverse of the node's transform
	 */
	const Transform& get_transform() const;
	Transform& get_transform();
	const Transform& get_inv_transform() const;
	Transform& get_inv_transform();
	const std::string& get_name() const;
	
private:
	/*
	 * Flatten the nodes children down into the vector passed in
	 * The nodes being flattened in will also clear their children
	 */
	void flatten_children(std::vector<std::shared_ptr<Node>> &nodes);
};

#endif

