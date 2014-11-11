#include <regex>
#include <fstream>
#include <array>
#include <map>
#include <cstdio>
#include <vector>
#include <string>
#include "linalg/vector.h"
#include "linalg/point.h"
#include "linalg/util.h"
#include "monte_carlo/util.h"
#include "monte_carlo/distribution1d.h"
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
bool Triangle::intersect(Ray &ray, DifferentialGeometry &diff_geom) const {
	const Point &pa = mesh->vertex(a);
	const Point &pb = mesh->vertex(b);
	const Point &pc = mesh->vertex(c);
	const std::array<Vector, 2> e{
		pb - pa,
		pc - pa
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
	std::array<float, 3> bary;
	bary[0] = d.dot(s[0]) * div;
	//Check that the first barycentric coordinate is in the triangle bounds
	if (bary[0] < -1e-8 || bary[0] > 1){
		return false;
	}
	s[1] = d.cross(e[0]);
	bary[1] = ray.d.dot(s[1]) * div;
	//Check the second barycentric coordinate is in the triangle bounds
	if (bary[1] < -1e-8 || bary[0] + bary[1] > 1){
		return false;
	}

	//We've hit the triangle with the ray, now check the hit location is in the ray range
	float t = e[1].dot(s[1]) * div;
	if (t < ray.min_t || t > ray.max_t){
		return false;
	}
	bary[2] = 1 - bary[0] - bary[1];
	ray.max_t = t;
	diff_geom.point = ray(t);

	const Normal &na = mesh->normal(a);
	const Normal &nb = mesh->normal(b);
	const Normal &nc = mesh->normal(c);
	diff_geom.normal = bary[2] * na + bary[0] * nb + bary[1] * nc;
	diff_geom.normal = diff_geom.normal.normalized();
	//Compute the geometric normal of the triangle
	diff_geom.geom_normal = Normal{e[0].cross(e[1]).normalized()};
	if (ray.d.dot(diff_geom.normal) < 0){
		diff_geom.hit_side = HITSIDE::FRONT;
	}
	else {
		diff_geom.hit_side = HITSIDE::BACK;
	}

	//Compute parameterization of surface and various derivatives for texturing
	//Triangles are parameterized by the obj texcoords at the vertices
	const Point &ta = mesh->texcoord(a);
	const Point &tb = mesh->texcoord(b);
	const Point &tc = mesh->texcoord(c);

	//Triangle points can be found by p_i = p_0 + u_i dp/du + v_i dp/dv
	//we use this property to find the derivatives dp/du and dp/dv
	std::array<float, 2> du{ta.x - tc.x, tb.x - tc.x};
	std::array<float, 2> dv{ta.y - tc.y, tb.y - tc.y};
	float det = du[0] * dv[1] - dv[0] * du[1];
	//If the texcoords are degenerate pick arbitrary coordinate system
	if (det == 0){
		coordinate_system(e[1].cross(e[0]).normalized(), diff_geom.dp_du,
			diff_geom.dp_dv);
		diff_geom.dn_du = Normal{0, 0, 0};
		diff_geom.dn_dv = Normal{0, 0, 0};
	}
	else {
		det = 1 / det;
		std::array<Vector, 2> dp{pa - pc, pb - pc};
		diff_geom.dp_du = (dv[1] * dp[0] - dv[0] * dp[1]) * det;
		diff_geom.dp_dv = (-du[1] * dp[0] + du[0] * dp[1]) * det;

		std::array<Normal, 2> dn{na - nc, nb - nc};
		diff_geom.dn_du = (dv[1] * dn[0] - dv[0] * dn[1]) * det;
		diff_geom.dn_dv = (-du[1] * dn[0] + du[0] * dn[1]) * det;
	}
	diff_geom.u = bary[2] * ta.x + bary[0] * tb.x + bary[1] * tc.x;
	diff_geom.v = bary[2] * ta.y + bary[0] * tb.y + bary[1] * tc.y;
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
float Triangle::surface_area() const {
	const Point &pa = mesh->vertex(a);
	const Point &pb = mesh->vertex(b);
	const Point &pc = mesh->vertex(c);
	return 0.5 * (pb - pa).cross(pc - pa).length();
}
Point Triangle::sample(const GeomSample &gs, Normal &normal) const {
	const Point &pa = mesh->vertex(a);
	const Point &pb = mesh->vertex(b);
	const Point &pc = mesh->vertex(c);
	const Normal &na = mesh->normal(a);
	const Normal &nb = mesh->normal(b);
	const Normal &nc = mesh->normal(c);
	Vector bary = uniform_sample_tri(gs.u);
	normal = na * bary.x + nb * bary.y + nc * bary.z;
	return pa * bary.x + pb * bary.y + pc * bary.z;
}

TriMesh::TriMesh(const std::string &file, bool no_bobj){
	load_model(file, no_bobj);
	refine_tris();
	std::vector<Geometry*> ref_tris;
	refine(ref_tris);
	bvh = BVH{ref_tris, SPLIT_METHOD::SAH, 32};
}
TriMesh::TriMesh(const std::vector<Point> &verts, const std::vector<Point> &tex,
	const std::vector<Normal> &norm, const std::vector<int> vert_idx)
	: vertices(verts), texcoords(tex), normals(norm), vert_indices(vert_idx)
{
	refine_tris();
	std::vector<Geometry*> ref_tris;
	refine(ref_tris);
	bvh = BVH{ref_tris, SPLIT_METHOD::SAH, 32};
}
bool TriMesh::intersect(Ray &ray, DifferentialGeometry &diff_geom) const {
	return bvh.intersect(ray, diff_geom);
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
float TriMesh::surface_area() const {
	return total_area;
}
Point TriMesh::sample(const GeomSample &gs, Normal &normal) const {
	return Point{0, 0, 0};
}
Point TriMesh::sample(const Point &p, const GeomSample &gs, Normal &normal) const {
	return Point{0, 0, 0};
}
float TriMesh::pdf(const Point &p) const {
	return 0;
}
float TriMesh::pdf(const Point &p, const Vector &w_i) const {
	return 0;
}
bool TriMesh::attach_light(const Transform &to_world){
	//Move the mesh into world space so we can get rid of any scaling and have proper
	//surface area computation
	for (auto &p : vertices){
		p = to_world(p);
	}
	for (auto &n : normals){
		n = to_world(n);
	}
	total_area = 0;
	for (const auto &t : tris){
		total_area += t.surface_area();
	}
	//Re-build the BVH in world space
	std::vector<Geometry*> ref_tris;
	refine(ref_tris);
	bvh = BVH{ref_tris, SPLIT_METHOD::SAH, 32};
	return true;
}
void TriMesh::refine_tris(){
	total_area = 0;
	tris.reserve(vert_indices.size());
	for (int i = 0; i < vert_indices.size(); i += 3){
		tris.emplace_back(vert_indices[i], vert_indices[i + 1], vert_indices[i + 2], this);
		total_area += tris.back().surface_area();
	}
}
void TriMesh::load_model(const std::string &file, bool no_bobj){
	//First see if a binary obj file is available, if not fall back to wavefront obj
	std::string file_bin = file.substr(0, file.rfind("obj")) + "bobj";
	std::ifstream fbin{file_bin};
	if (no_bobj || !fbin.good()){
		std::ifstream fin{file};
		if (!fin.good()){
			std::cout << "Error: failed to load model " << file << std::endl;
			return;
		}
		else {
			fin.close();
			load_wobj(file);
		}
	}
	else {
		std::cout << "Found optimized binary mesh file " << file_bin << std::endl;
		fbin.close();
		load_bobj(file_bin);
	}
}
void TriMesh::load_wobj(const std::string &file){
	std::ifstream fin{file};
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
}
void TriMesh::load_bobj(const std::string &file){
	std::FILE *fin = std::fopen(file.c_str(), "rb");
	uint32_t nverts = 0, ntris = 0;
	std::fread(&nverts, sizeof(uint32_t), 1, fin);
	std::fread(&ntris, sizeof(uint32_t), 1, fin);
	vertices.resize(nverts);
	texcoords.resize(nverts);
	normals.resize(nverts);
	vert_indices.resize(3 * ntris);
	std::fread(vertices.data(), sizeof(Point), nverts, fin);
	std::fread(texcoords.data(), sizeof(Point), nverts, fin);
	std::fread(normals.data(), sizeof(Normal), nverts, fin);
	std::fread(vert_indices.data(), sizeof(int), 3 * ntris, fin);
	std::fclose(fin);
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
	std::regex match_vert{"([0-9]+)/([0-9]+)/([0-9]+)"};
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

