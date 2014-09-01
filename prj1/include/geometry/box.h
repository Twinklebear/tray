#ifndef BOX_H
#define BOX_H

#include "geometry.h"

/*
 * A box spanning [-1, -1, -1] to [1, 1, 1] centered at the origin
 */
class Box : public Geometry {
public:
	bool intersect(Ray &r) override;
};

#endif

