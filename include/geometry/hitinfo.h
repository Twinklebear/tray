#ifndef HIT_INFO_H
#define HIT_INFO_H

#include "linalg/point.h"
#include "linalg/vector.h"

class Node;

enum HITSIDE { NONE, FRONT, BACK };

/*
 * Stores information about the geometry at a hit location
 * and a pointer to the node that was hit so we can
 * grab the material to shade
 */
struct HitInfo {
	Point point;
	Normal normal;
	const Node *node;
	HITSIDE hit_side;

	HitInfo(const Point &point = Point{}, const Normal &normal = Normal{},
		const Node *node = nullptr, HITSIDE hit_side = NONE);
};

#endif

