#ifndef RAY_H
#define RAY_H

#include <limits>
#include <ostream>
#include "vector.h"
#include "point.h"

class DifferentialGeometry;

/*
 * A ray in 3D space starting at o and in direction d
 */
struct Ray {
	Point o;
	Vector d;
	//min/max t range of the ray
	float min_t, max_t;
	//recursion depth of this ray, needed for some algorithms
	int depth;
	float time;

	inline Ray(const Point &o = Point{}, const Vector &d = Vector{}, float min_t = 0,
		float max_t = std::numeric_limits<float>::infinity(), int depth = 0, float time = 0)
		: o(o), d(d), min_t(min_t), max_t(max_t), depth(depth), time(time)
	{}
	/*
	 * Use to indicate that some ray has spawned this one,
	 * increasing the recursion depth
	 */
	inline Ray(const Point &o, const Vector &d, const Ray &parent, float min_t = 0,
		float max_t = std::numeric_limits<float>::infinity())
		: o(o), d(d), min_t(min_t), max_t(max_t), depth(parent.depth + 1), time(parent.time)
	{}
	//Get a point at some t along the ray
	inline Point operator()(float t) const {
		return o + t * d;
	}
};
inline std::ostream& operator<<(std::ostream &os, const Ray &r){
	os << "r ( o = " << r.o << ", d = " << r.d
		<< ", t = { " << r.min_t << ", " << r.max_t
		<< "}, depth = " << r.depth << ")";
	return os;
}

/*
 * A ray + 2 offset rays (x, y) for use in computing texture anti-aliasing
 */
struct RayDifferential : Ray {
	//Rays one sample over in x/y
	Ray rx, ry;

	inline RayDifferential(const Point &o = Point{}, const Vector &d = Vector{}, float min_t = 0,
		float max_t = std::numeric_limits<float>::infinity(), int depth = 0, float time = 0)
		: Ray(o, d, min_t, max_t, depth, time)
	{}
	/*
	 * Use to indicate that some ray has spawned this one,
	 * increasing the recursion depth
	 */
	inline RayDifferential(const Point &o, const Vector &d, const Ray &parent, float min_t = 0,
		float max_t = std::numeric_limits<float>::infinity())
		: Ray(o, d, parent, min_t, max_t)
	{}
	inline explicit RayDifferential(const Ray &r) : Ray{r} {}
	inline void scale_differentials(float s){
		rx.o = o + (rx.o - o) * s;
		ry.o = o + (ry.o - o) * s;
		rx.d = d + (rx.d - d) * s;
		ry.d = d + (ry.d - d) * s;
	}
	/*
	 * Reflect the ray and its differentials off the piece of geometry
	 */
	RayDifferential reflect(const DifferentialGeometry &dg) const;
	/*
	 * Refract the ray and its differentials through the piece of geometry
	 * n: the surface normal corrected to face towards the incident ray
	 * eta: incident index of refraction / transmitted index of refraction
	 */
	RayDifferential refract(const DifferentialGeometry &dg, float eta) const;
	inline bool has_differentials() const {
		return rx.d.length_sqr() > 0 && ry.d.length_sqr() > 0;
	}
};
inline std::ostream& operator<<(std::ostream &os, const RayDifferential &r){
	os << "rdiff ( " << static_cast<const Ray&>(r) << "\n\trx = "
		<< r.rx << "\n\try = " << r.ry << " )";
	return os;
}

#endif

