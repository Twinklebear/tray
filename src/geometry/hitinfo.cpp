#include "linalg/point.h"
#include "linalg/vector.h"
#include "geometry/geometry.h"
#include "geometry/hitinfo.h"

HitInfo::HitInfo(const Point &point, const Normal &normal, const Node *node, HITSIDE hit_side)
	: point(point), normal(normal), node(node), hit_side(hit_side)
{}

