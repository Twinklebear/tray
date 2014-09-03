#ifndef BOX_H
#define BOX_H

#include "geometry.h"

/*
 * A box spanning [-1, -1, -1] to [1, 1, 1] centered at the origin
 */
class Box : public Geometry {
public:
	bool intersect(Ray &ray, HitInfo &hitinfo) override;

private:
	/*
	 * Compute the box normal at the point on the surface and return it
	 */
	Normal normal_at(const Point &p);
};

#endif

