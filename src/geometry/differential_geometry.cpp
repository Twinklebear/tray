#include "linalg/point.h"
#include "linalg/vector.h"
#include "geometry/geometry.h"
#include "geometry/differential_geometry.h"

DifferentialGeometry::DifferentialGeometry() : node(nullptr), hit_side(NONE), u(0), v(0), du_dx(0), dv_dx(0),
	du_dy(0), dv_dy(0)
{}
DifferentialGeometry::DifferentialGeometry(const Point &point, const Vector &dp_du, const Vector &dp_dv,
	const Normal &dn_du, const Normal &dn_dv, const Normal &normal, float u, float v,
	const Node *node, HITSIDE hit_side)
	: point(point), normal(normal), node(node), hit_side(hit_side), dp_du(dp_du), dp_dv(dp_dv),
	dn_du(dn_du), dn_dv(dn_dv), u(u), v(v), du_dx(0), dv_dx(0), du_dy(0), dv_dy(0)
{}

