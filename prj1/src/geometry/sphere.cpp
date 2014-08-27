#include "linalg/ray.h"
#include "linalg/vector.h"
#include "geometry/sphere.h"

Sphere::Sphere(const Point &center, float radius) : center(center), radius(radius){}
bool Sphere::intersect(Ray &ray){
	Vector l = center - ray.o;
	float l_sqr = l.dot(l);
	//s is l projected along the ray direction to the sphere
	float s = l.dot(ray.d);
	//Gonna need radius^2 a lot, so pre-compute it
	float r_sqr = radius * radius;

	//If s < 0 and ray origin is out of the sphere then the sphere is behind the ray
	if (s < 0 && l_sqr > r_sqr){
		return false;
	}

	//m is the final piece of the triangle spanned by l & s
	//if the distance is further than the radius we definitely don't hit
	float m_sqr = l_sqr - s * s;
	if (m_sqr > r_sqr){
		return false;
	}

	//Definitely have a hit, find out where by completing the triangle
	//spanned by m & r, then use l_sqr and r_sqr to determine if we're hitting
	//the sphere from the outside or if the ray originated inside and we're exiting
	float q = std::sqrt(r_sqr - m_sqr);
	float t = l_sqr > r_sqr ? s - q : s + q;
	//If this is the closest object hit by the ray so far, count the hit
	if (t < ray.max_t){
		ray.max_t = t;
		return true;
	}
	return false;
}

