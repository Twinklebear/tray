#ifndef TRI_MESH_H
#define TRI_MESH_H

#include <vector>
#include <string>
#include "linalg/vector.h"
#include "linalg/point.h"
#include "linalg/ray.h"
#include "monte_carlo/distribution1d.h"
#include "bbox.h"
#include "geometry.h"
#include "accelerators/bvh.h"
#include "mesh_preprocess.h"

class TriMesh;

/*
 * A single triangle in the TriMesh, just holds
 * its 3 vertex indices and a non-owning pointer to the mesh
 */
class Triangle : public Geometry {
	int a, b, c;
	const TriMesh *mesh;

public:
	Triangle(int a = 0, int b = 0, int c = 0, const TriMesh *mesh = nullptr);
	bool intersect(Ray &ray, DifferentialGeometry &diff_geom) const override;
	BBox bound() const override;
	void refine(std::vector<Geometry*> &prims) override;
	/*
	 * Compute the surface area of the sphere
	 */
	float surface_area() const override;
	/*
	 * Sample a position on the geometry and return the point and normal
	 */
	Point sample(const GeomSample &gs, Normal &normal) const override;
};

/*
 * A mesh composed of triangles
 */
class TriMesh : public Geometry {
	std::vector<Point> vertices, texcoords;
	std::vector<Normal> normals;
	//Indices for each face's vert, texcoord and normal
	//We could do better by storing 3 indices one for each vert, texcoord and normal
	std::vector<int> vert_indices;
	//Triangles for the mesh, cached after the first time the mesh is refined
	//since we hand out references to them
	std::vector<Triangle> tris;
	//The BVH used to accelerate ray-triangle intersection tests on the mesh
	BVH bvh;
	//The total surface area of the mesh
	float total_area;
	//1D distribution of the triangle area values, only computed if a light
	//is attached
	Distribution1D area_distribution;

	//Friends with the meshprocessor so it's able to get the data needed
	//to serialize the binary mesh
	friend bool process_wobj(const std::string &);

public:
	/*
	 * Load the mesh from a Wavefront OBJ file, both triangle and quad faces
	 * are supported although quad faces will simply be split into triangles
	 * during loading
	 * Can optionally request that any binary obj files found are ignored
	 * This is only used by the mesh preprocessor to not load & process any
	 * existing binary files
	 */
	TriMesh(const std::string &file, bool no_bobj = false);
	/*
	 * Explicitly specify the mesh information for the model
	 */
	TriMesh(const std::vector<Point> &verts, const std::vector<Point> &tex,
		const std::vector<Normal> &norm, const std::vector<int> vert_idx);
	bool intersect(Ray &ray, DifferentialGeometry &diff_geom) const override;
	BBox bound() const override;
	void refine(std::vector<Geometry*> &prims) override;
	/*
	 * Get a vertex, texcoord or normal at the desired index
	 */
	const Point& vertex(int i) const;
	const Point& texcoord(int i) const;
	const Normal& normal(int i) const;
	/*
	 * Compute the surface area of the sphere
	 */
	float surface_area() const override;
	/*
	 * Sample a position on the geometry and return the point and normal
	 */
	Point sample(const GeomSample &gs, Normal &normal) const override;
	/*
	 * Sample the shape using the probability density of the solid angle from
	 * point p to the point on the surface
	 */
	Point sample(const Point &p, const GeomSample &gs, Normal &normal) const override;
	/*
	 * Compute the pdf of sampling uniformly on the surface
	 */
	float pdf(const Point &p) const override;
	/*
	 * Compute the pdf that the ray from p with direction w_i intersects the shape
	 */
	float pdf(const Point &p, const Vector &w_i) const override;
	/*
	 * Alert the geometry that an area light has been attached to it, passing its transform
	 * as well. The triangle mesh uses the transform to move itself into world space
	 * returns true if the light can be attached, false if a light can't be attached
	 */
	bool attach_light(const Transform &to_world) override;

private:
	/*
	 * Refine the mesh down to its component triangles by computing them
	 * and cacheing them
	 */
	void refine_tris();
	/*
	 * Load the model from a wavefront obj file or a binary obj file
	 * depending on what's available, preferring binary obj files
	 * bobj indicates if we want to ignore any binary object files
	 * only used by the mesh preprocessor to not load & process any
	 * existing binary files
	 */
	void load_model(const std::string &file, bool no_bobj = false);
	/*
	 * Load the mesh data from a wavefront obj file
	 */
	void load_wobj(const std::string &file);
	/*
	 * Load the mesh data from a preprocessed binary obj file
	 */
	void load_bobj(const std::string &file);
};

#endif

