#ifndef BBOX_H
#define BBOX_H

#include <array>
#include <ostream>
#include <cmath>
#include <limits>
#include "linalg/util.h"
#include "linalg/point.h"
#include "linalg/ray.h"

/*
 * An AABB
 */
struct BBox {
	Point min, max;

	inline BBox()
		: min(Point{std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(),
			std::numeric_limits<float>::infinity()}),
		max(Point{-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(),
			std::numeric_limits<float>::infinity()})
	{}
	inline BBox(const Point &p) : min(p), max(p) {}
	inline BBox(const Point &a, const Point &b)
		: min(Point{std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z)}),
		max(Point{std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z)})
	{}
	inline void expand(float delta){
		min -= Vector{delta};
		max += Vector{delta};
	}
	/*
	 * Get a box representing the union of this box and another
	 */
	inline BBox box_union(const BBox &b) const {
		BBox u = *this;
		u.min = Point{std::min(u.min.x, b.min.x), std::min(u.min.y, b.min.y),
			std::min(u.min.z, b.min.z)};
		u.max = Point{std::max(u.max.x, b.max.x), std::max(u.max.y, b.max.y),
			std::max(u.max.z, b.max.z)};
		return u;
	}
	/*
	 * Get a box representing this box expanded to contain the point
	 */
	inline BBox box_union(const Point &p) const {
		BBox u = *this;
		u.min = Point{std::min(u.min.x, p.x), std::min(u.min.y, p.y),
			std::min(u.min.z, p.z)};
		u.max = Point{std::max(u.max.x, p.x), std::max(u.max.y, p.y),
			std::max(u.max.z, p.z)};
		return u;
	}
	inline bool overlaps(const BBox &b) const {
		bool x = max.x >= b.min.x && min.x <= b.max.x;
		bool y = max.y >= b.min.y && min.y <= b.max.y;
		bool z = max.z >= b.min.z && min.z <= b.max.z;
		return x && y && z;
	}
	inline bool inside(const Point &p) const {
		return p.x >= min.x && p.x <= max.x && p.y >= min.y
			&& p.y <= max.y && p.z >= min.z && p.z <= max.z;
	}
	inline float surface_area() const {
		Vector d = max - min;
		return 2.f * (d.x * d.y + d.x * d.z + d.y * d.z);
	}
	inline float volume() const {
		Vector d = max - min;
		return d.x * d.y * d.z;
	}
	/*
	 * Return which axis is the longest
	 */
	inline int max_extent() const {
		Vector d = max - min;
		if (d.x > d.y && d.x > d.z){
			return 0;
		}
		else if (d.y > d.z){
			return 1;
		}
		return 2;
	}
	/*
	 * Blend between the min/max points to get some point in the box
	 */
	inline Point lerp(float tx, float ty, float tz) const {
		return Point{::lerp(tx, min.x, max.x), ::lerp(ty, min.y, max.y),
			::lerp(tz, min.z, max.z)};
	}
	/*
	 * Find the position of the point relative to the box, with min being the origin
	 */
	inline Vector offset(const Point &p) const {
		return Vector{(p.x - min.x) / (max.x - min.x),
			(p.y - min.y) / (max.y - min.y),
			(p.z - min.z) / (max.z - min.z)};
	}
	inline const Point& operator[](size_t i) const {
		return i == 0 ? min : max;
	}
	inline Point& operator[](size_t i){
		return i == 0 ? min : max;
	}
	/*
	 * Test if the ray intersects the box
	 */
	inline bool intersect(const Ray &ray) const {
		std::array<float, 2> t = { ray.min_t, ray.max_t };
		//Check each slab of the bbox
		for (int i = 0; i < 3; ++i){
			float inv_dir = 1 / ray.d[i];
			float tnear = (min[i] - ray.o[i]) * inv_dir;
			float tfar = (max[i] - ray.o[i]) * inv_dir;
			if (tnear > tfar){
				std::swap(tnear, tfar);
			}
			t[0] = tnear > t[0] ? tnear : t[0];
			t[1] = tfar < t[1] ? tfar : t[1];
			if (t[0] > t[1]){
				return false;
			}
		}
		return true;
	}
};
inline std::ostream& operator<<(std::ostream &os, const BBox &b){
	os << "BBox { min=" << b.min << ", max=" << b.max;
	return os;
}

#endif

