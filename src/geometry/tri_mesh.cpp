#include <vector>
#include <string>
#include "linalg/vector.h"
#include "linalg/point.h"
#include "geometry/bbox.h"
#include "geometry/geometry.h"
#include "geometry/tri_mesh.h"

TriMesh::TriMesh(const std::string &file){
	load_wobj(file);
}
TriMesh::TriMesh(const std::vector<Point> &verts, const std::vector<Point> &tex,
	const std::vector<Normal> &norm, const std::vector<int> vert_idx)
	: verts(verts), texcoords(tex), normals(norm), vert_indices(vert_idx)
{
	compute_bounds();
}
bool TriMesh::intersect(Ray &ray, HitInfo &hitinfo){
	//Really terrible method, go through all the triangles in the mesh
	//and see if we hit them
	bool hit = false;
	for (int i = 0; i < vert_indices.size(); i += 3){
		Triangle t{vert_indices[i], vert_indices[i + 1], vert_indices[i + 2], this};
		hit = t.intersect(ray, hitinfo) || hit;
	}
	return hit;
}
BBox TriMesh::object_bound() const {
	return bounds;
}
const Point& TriMesh::vertex(int i) const {
	return verts[i];
}
const Point& TriMesh::texcoord(int i) const {
	return texcoords[i];
}
const Normal& TriMesh::normal(int i) const {
	return normals[i];
}
void TriMesh::compute_bounds(){
	bounds = BBox{Point{0, 0, 0}, Point{0, 0, 0}};
	for (const Point &p : verts){
		bounds = bounds.box_union(p);
	}
}
void TriMesh::load_wobj(const std::string &file){
}

Triangle::Triangle(int a, int b, int c, const TriMesh *mesh) : a(a), b(b), c(c), mesh(mesh){}
bool Triangle::intersect(Ray &ray, HitInfo &hitinfo){
	const std::array<Vector, 2> e = {
		mesh->vertex(b) - mesh->vertex(a),
		mesh->vertex(c) - mesh->vertex(a)
	};
	Vector dir = ray.d.normalized();
	std::array<Vector, 2> s;
	s[0] = dir.cross(e[1]);
	float div = s[0].dot(e[0]);
	//Check for degenerate triangle
	if (div == 0){
		return false;
	}
	div = 1.f / div;
	Vector d = ray.o - mesh->vertex(a);
	std::array<float, 2> bary;
	bary[0] = d.dot(s[0]) * div;
	//Check that the first barycentric coordinate is in the triangle bounds
	if (bary[0] < 0 || bary[0] > 1){
		return false;
	}
	s[1] = d.cross(e[0]);
	bary[1] = dir.dot(s[1]) * div;
	//Check the second barycentric coordinate is in the triangle bounds
	if (bary[1] < 0 || bary[0] + bary[1] > 1){
		return false;
	}

	//We've hit the triangle with the ray, now check the hit location is in the ray range
	float t = e[1].dot(s[1]) * div;
	if (t < ray.min_t || t > ray.max_t){
		return false;
	}
	ray.max_t = t;
	hitinfo.point = ray(t);
	hitinfo.normal = (1 - bary[0] - bary[1]) * mesh->normal(a) + bary[0] * mesh->normal(b)
		+ bary[1] * mesh->normal(c);
	hitinfo.normal = hitinfo.normal.normalized();
	return true;

}
BBox Triangle::object_bound() const {
	BBox box{mesh->vertex(a), mesh->vertex(b)};
	return box.box_union(mesh->vertex(c));
}

