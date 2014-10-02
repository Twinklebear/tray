#include "linalg/point.h"
#include "linalg/vector.h"
#include "linalg/ray.h"
#include "linalg/util.h"
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
void DifferentialGeometry::compute_differentials(const RayDifferential &r){
	if (r.has_differentials()){
		//Compute the intersection points of the ray differentials with the plane of the
		//hit geometry (eg the plane at point with normal)
		float d = -normal.dot(Vector{point});
		float t = -(normal.dot(Vector{r.rx.o}) + d) / normal.dot(r.rx.d);
		Point px = r.rx(t);
		t = -(normal.dot(Vector{r.ry.o}) + d) / normal.dot(r.ry.d);
		Point py = r.ry(t);
		dp_dx = px - point;
		dp_dy = py - point;

		//Solve linear system with px and py to compute the various parameterization differentials
		//be careful to choose the system to not be degenerate
		std::array<int, 2> axes;
		if (std::abs(normal.x) > std::abs(normal.y) && std::abs(normal.x) > std::abs(normal.z)){
			axes[0] = AXIS::Y;
			axes[1] = AXIS::Z;
		}
		else if (std::abs(normal.y) > std::abs(normal.z)){
			axes[0] = AXIS::X;
			axes[1] = AXIS::Z;
		}
		else {
			axes[0] = AXIS::X;
			axes[1] = AXIS::Y;
		}
		axes[0] = AXIS::X;
		axes[1] = AXIS::Y;
		std::array<float, 4> mat{
			dp_du[axes[0]], dp_dv[axes[0]],
			dp_du[axes[1]], dp_dv[axes[1]]
		};
		std::array<float, 2> bx{
			px[axes[0]] - point[axes[0]],
			px[axes[1]] - point[axes[1]]
		};
		std::array<float, 2> by{
			py[axes[0]] - point[axes[0]],
			py[axes[1]] - point[axes[1]]
		};
		if (!solve_linear2x2(mat, bx, du_dx, dv_dx)){
			du_dx = 0;
			dv_dx = 0;
		}
		if (!solve_linear2x2(mat, by, du_dy, dv_dy)){
			du_dy = 0;
			dv_dy = 0;
		}
	}
	else {
		du_dx = 0;
		dv_dx = 0;
		du_dy = 0;
		dv_dy = 0;
		dp_dx = Vector{0};
		dp_dy = Vector{0};
	}
}

