#ifndef TRI_MESH_H
#define TRI_MESH_H

#include <vector>
#include <string>
#include "linalg/vector.h"
#include "linalg/point.h"
#include "linalg/ray.h"
#include "bbox.h"
#include "geometry.h"

class TriMesh;

/*
 * A single triangle in the TriMesh, just holds
 * its 3 vertex indices and a non-owning pointer to the mesh
 */
class Triangle : public Geometry {
	int a, b, c;
	const TriMesh *mesh;

public:
	Triangle(int a, int b, int c, const TriMesh *mesh);
	bool intersect(Ray &ray, HitInfo &hitinfo) override;
	BBox bound() const override;
	void refine(std::vector<Geometry*> &prims) override;
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
	//The mesh computes its object-space bounds once after loading and saves it
	BBox bounds;

public:
	/*
	 * Load the mesh from a Wavefront OBJ file, both triangle and quad faces
	 * are supported although quad faces will simply be split into triangles
	 * during loading
	 */
	TriMesh(const std::string &file);
	/*
	 * Explicitly specify the mesh information for the model
	 */
	TriMesh(const std::vector<Point> &verts, const std::vector<Point> &tex,
		const std::vector<Normal> &norm, const std::vector<int> vert_idx);
	bool intersect(Ray &ray, HitInfo &hitinfo) override;
	BBox bound() const override;
	void refine(std::vector<Geometry*> &prims) override;
	/*
	 * Get a vertex, texcoord or normal at the desired index
	 */
	const Point& vertex(int i) const;
	const Point& texcoord(int i) const;
	const Normal& normal(int i) const;

private:
	/*
	 * Compute the object space AABB for this mesh, done once after loading
	 * or setting explicitly the mesh data
	 */
	void compute_bounds();
	/*
	 * Refine the mesh down to its component triangles by computing them
	 * and cacheing them
	 */
	void refine_tris();
	/*
	 * Load the mesh data from a wavefront obj file
	 */
	void load_wobj(const std::string &file);
};

#endif

