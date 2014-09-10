#ifndef DBG_TRI_H
#define DBG_TRI_H

#include "geometry.h"

/*
 * A lone triangle for debugging triangle intersection tests
 * has coords {0, 1, 0}, {0, 0, 0}, {1, 0, 0}
 * and normal along the z axis
 */
class DebugTri : public Geometry {
public:
	bool intersect(Ray &ray, HitInfo &hitinfo) override;
	BBox object_bound() const override;
};

#endif

