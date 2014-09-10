#include <array>
#include "linalg/point.h"
#include "linalg/vector.h"
#include "geometry/geometry.h"
#include "geometry/dbg_tri.h"

bool DebugTri::intersect(Ray &ray, HitInfo &hitinfo) {
	const static std::array<Point, 3> verts = {
		Point{0, 0, 0}, Point{1, 0, 0}, Point{0, 1, 0}
	};
	const static Normal normal{0, 0, 1};

	const std::array<Vector, 2> e = {
		verts[1] - verts[0], verts[2] - verts[0]
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
	Vector d = ray.o - verts[0];
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
	hitinfo.normal = normal;
	return true;
}

