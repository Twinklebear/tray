#include <regex>
#include <fstream>
#include <array>
#include <map>
#include <cstdio>
#include <vector>
#include <string>
#include "linalg/vector.h"
#include "linalg/point.h"
#include "accelerators/bvh.h"
#include "geometry/bbox.h"
#include "geometry/geometry.h"
#include "geometry/tri_mesh.h"

//Various capture utilities for loading the wavefront obj format
static Point capture_point2(const std::string &s);
static Point capture_point3(const std::string &s);
static Normal capture_normal(const std::string &s);
static std::vector<std::string> capture_face(const std::string &s);
static std::array<int, 3> capture_vertex(const std::string &s);

Triangle::Triangle(int a, int b, int c, const TriMesh *mesh) : a(a), b(b), c(c), mesh(mesh){}
bool Triangle::intersect(Ray &ray, HitInfo &hitinfo){
	const std::array<Vector, 2> e = {
		mesh->vertex(b) - mesh->vertex(a),
		mesh->vertex(c) - mesh->vertex(a)
	};
	std::array<Vector, 2> s;
	s[0] = ray.d.cross(e[1]);
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
	bary[1] = ray.d.dot(s[1]) * div;
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
BBox Triangle::bound() const {
	BBox box;
	return box.box_union(mesh->vertex(a)).box_union(mesh->vertex(b))
		.box_union(mesh->vertex(c));
}
void Triangle::refine(std::vector<Geometry*> &prims){
	prims.push_back(this);
}

TriMesh::TriMesh(const std::string &file){
	load_wobj(file);
	std::vector<Geometry*> ref_tris;
	refine(ref_tris);
	bvh = BVH{ref_tris, SPLIT_METHOD::SAH, 128};
}
TriMesh::TriMesh(const std::vector<Point> &verts, const std::vector<Point> &tex,
	const std::vector<Normal> &norm, const std::vector<int> vert_idx)
	: vertices(verts), texcoords(tex), normals(norm), vert_indices(vert_idx)
{
	compute_bounds();
	refine_tris();
	std::vector<Geometry*> ref_tris;
	refine(ref_tris);
	bvh = BVH{ref_tris, SPLIT_METHOD::SAH, 128};
}
bool TriMesh::intersect(Ray &ray, HitInfo &hitinfo){
	return bvh.intersect(ray, hitinfo);
	/*
	//TODO Swap in crappy slow intersection code for the nice bvh when doing
	//project 5 since we aren't supposed to have it yet
	//Really terrible method, go through all the triangles in the mesh
	//and see if we hit them
	bool hit = false;
	for (Triangle &t : tris){
		hit = t.intersect(ray, hitinfo) || hit;
	}
	return hit;
	*/
}
BBox TriMesh::bound() const {
	return bvh.bounds();
}
void TriMesh::refine(std::vector<Geometry*> &prims){
	for (Triangle &t : tris){
		prims.push_back(&t);
	}
}
const Point& TriMesh::vertex(int i) const {
	return vertices[i];
}
const Point& TriMesh::texcoord(int i) const {
	return texcoords[i];
}
const Normal& TriMesh::normal(int i) const {
	return normals[i];
}
void TriMesh::compute_bounds(){
	/*
	bounds = BBox{};
	for (const Point &p : vertices){
		bounds = bounds.box_union(p);
	}
	*/
}
void TriMesh::refine_tris(){
	for (int i = 0; i < vert_indices.size(); i += 3){
		tris.emplace_back(vert_indices[i], vert_indices[i + 1], vert_indices[i + 2], this);
	}
}
void TriMesh::load_wobj(const std::string &file){
	std::ifstream fin(file);
	if (!fin.is_open()){
		std::cout << "Error: failed to load obj file: " << file << std::endl;
		return;
	}
	//Temporary storage for the data we read in
	//we could skip doing this if we stored 3 indices per tri {pos, normal, tex} instead of just 1
	std::vector<Point> tmp_pos, tmp_uv;
	std::vector<Normal> tmp_norm;
	//A map to associate a unique vertex (#/#/#) with its index
	std::map<std::string, int> idx;
	
	std::string line;
	while (std::getline(fin, line)){
		if (line.empty()){
			continue;
		}
		//Parse in position, normal and texcoord information
		if (line.at(0) == 'v'){
			//positions
			if (line.at(1) == ' '){
				tmp_pos.push_back(capture_point3(line));
			}
			//texcoords
			else if (line.at(1) == 't'){
				tmp_uv.push_back(capture_point2(line));
			}
			//normals
			else if (line.at(1) == 'n'){
				tmp_norm.push_back(capture_normal(line));
			}
		}
		//Parse faces
		else if (line.at(0) == 'f'){
			std::vector<std::string> face = capture_face(line);
			//Triangulate quad faces
			if (face.size() == 4){
				face.push_back(face.at(0));
				face.push_back(face.at(2));
			}
			for (std::string &v : face){
				auto fnd = idx.find(v);
				//If we already have the vertex indexed then re-use the index
				if (fnd != idx.end()){
					vert_indices.push_back(fnd->second);
				}
				else {
					std::array<int, 3> vt = capture_vertex(v);
					//Note: Wavefront OBJ uses 1 based indices
					vertices.push_back(tmp_pos[vt[0] - 1]);
					texcoords.push_back(tmp_uv[vt[1] - 1]);
					normals.push_back(tmp_norm[vt[2] - 1]);
					//Store the new index for this vertex in the map to re-use it
					vert_indices.push_back(vertices.size() - 1);
					idx[v] = vert_indices.back();
				}
			}
		}
	}
	compute_bounds();
	refine_tris();
}
Point capture_point2(const std::string &s){
	Point p;
	std::sscanf(s.c_str(), "%*s %f %f", &p.x, &p.y);
	return p;
}
Point capture_point3(const std::string &s){
	Point p;
	std::sscanf(s.c_str(), "%*s %f %f %f", &p.x, &p.y, &p.z);
	return p;
}
Normal capture_normal(const std::string &s){
	Normal n;
	std::sscanf(s.c_str(), "%*s %f %f %f", &n.x, &n.y, &n.z);
	return n;
}
std::vector<std::string> capture_face(const std::string &s){
	std::regex match_vert("([0-9]+)/([0-9]+)/([0-9]+)");
	std::vector<std::string> face;
	std::transform(std::sregex_iterator{s.begin(), s.end(), match_vert},
		std::sregex_iterator{}, std::back_inserter(face),
		[](const std::smatch &m){
			return m.str();
		});
	return face;
}
std::array<int, 3> capture_vertex(const std::string &s){
	std::array<int, 3> vert;
	sscanf(s.c_str(), "%d/%d/%d", &vert[0], &vert[1], &vert[2]);
	return vert;
}

