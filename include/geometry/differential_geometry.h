#ifndef DIFFERENTIAL_GEOMETRY_H 
#define DIFFERENTIAL_GEOMETRY_H

#include "linalg/point.h"
#include "linalg/vector.h"
#include "linalg/ray.h"

class Node;

enum HITSIDE { NONE, FRONT, BACK };

/*
 * Stores information about the differential piece of geometry 
 * that was hit and a pointer to the node that was hit so we can
 * grab the material to shade this hit point
 */
struct DifferentialGeometry {
	Point point;
	Normal normal;
	const Node *node;
	HITSIDE hit_side;
	//Various derivatives and info we need for texture mapping and filtering
	Vector dp_du, dp_dv, dp_dx, dp_dy;
	Normal dn_du, dn_dv;
	float u, v, du_dx, dv_dx, du_dy, dv_dy;

	DifferentialGeometry();
	DifferentialGeometry(const Point &point, const Vector &dp_du, const Vector &dp_dv,
		const Normal &dn_du, const Normal &dn_dv, const Normal &normal, float u, float v,
		const Node *node, HITSIDE hit_side);
	/*
	 * Compute the (u, v) derivatives using information about the
	 * change in pixel position from the ray differential
	 */
	void compute_differentials(const RayDifferential &r);
};

#endif

