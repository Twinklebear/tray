#ifndef HIT_INFO_H
#define HIT_INFO_H

#include "linalg/point.h"
#include "linalg/vector.h"

class Node;

/*
 * Stores information about the geometry at a hit location
 * and a pointer to the node that was hit so we can
 * grab the material to shade
 */
struct HitInfo {
	Point point;
	Normal normal;
	const Node *node;

	HitInfo(const Point &point = Point{}, const Normal &normal = Normal{}, const Node *node = nullptr);
};

#endif

