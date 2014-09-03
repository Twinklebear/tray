#include <cmath>
#include <limits>
#include <array>
#include "linalg/util.h"
#include "linalg/vector.h"
#include "linalg/ray.h"
#include "geometry/box.h"

bool Box::intersect(Ray &ray, HitInfo &hitinfo){
	//The box half-vectors
	static const std::array<Vector, 3> axes{ Vector{1, 0, 0},
		Vector{0, 1, 0}, Vector{0, 0, 1}
	};
	float t_min = std::numeric_limits<float>::min();
	float t_max = std::numeric_limits<float>::max();
	//The vector from the ray origin to the box's center (0, 0, 0)
	Vector p{-ray.o};
	//Check which slab we're probably hitting by finding which half vector
	//p has the greatest length along
	for (size_t i = 0; i < axes.size(); ++i){
		float e = axes[i].dot(p);
		float f = axes[i].dot(ray.d);
		if (std::abs(f) > 1e-5){
			float t1 = (e + 1) / f;
			float t2 = (e - 1) / f;
			if (t1 > t2){
				std::swap(t1, t2);
			}
			if (t1 > t_min){
				t_min = t1;
			}
			if (t2 < t_max){
				t_max = t2;
			}
			if (t_min > t_max || t_max < 0){
				return false;
			}
		}
		else if (-e - 1 > 0 || -e + 1 < 0){
			return false;
		}
	}
	//We've hit the box, now check that it's in the range the ray is looking for
	if (t_min >= ray.min_t && t_min <= ray.max_t){
		ray.max_t = t_min;
		hitinfo.point = ray(t_min);
		hitinfo.normal = normal_at(hitinfo.point);
		return true;
	}
	else if (t_max >= ray.min_t && t_max <= ray.max_t){
		ray.max_t = t_max;
		hitinfo.point = ray(t_max);
		hitinfo.normal = normal_at(hitinfo.point);
		return true;
	}
	return false;
}
Normal Box::normal_at(const Point &p){
	float x = std::abs(p.x);
	float y = std::abs(p.y);
	float z = std::abs(p.z);
	if (x > y){
		if (x > z){
			return Normal{1, 0, 0} * sign(x);
		}
		else {
			return Normal{0, 0, 1} * sign(z);
		}
	}
	else {
		if (y > z){
			return Normal{0, 1, 0} * sign(y);
		}
		else {
			return Normal{0, 0, 1} * sign(z);
		}
	}
}

