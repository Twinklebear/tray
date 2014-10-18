#include "linalg/vector.h"
#include "linalg/point.h"
#include "geometry/differential_geometry.h"
#include "linalg/ray.h"

RayDifferential RayDifferential::reflect(const DifferentialGeometry &dg) const {
	//wo = -ray.d
	//wi is the reflected direction of the main ray, ie. refl.d
	Vector n{dg.normal.normalized()};
	Vector dir = d - 2 * n.dot(d) * n;
	RayDifferential refl{dg.point, dir.normalized(), *this, 0.001};
	if (has_differentials()){
		refl.rx = Ray{refl.o + dg.dp_dx, refl.d, *this, 0.001};
		refl.ry = Ray{refl.o + dg.dp_dy, refl.d, *this, 0.001};
		auto dn_dx = Vector{dg.dn_du * dg.du_dx + dg.dn_dv * dg.dv_dx};
		auto dn_dy = Vector{dg.dn_du * dg.du_dy + dg.dn_dv * dg.dv_dy};
		auto do_dx = -rx.d + d;
		auto do_dy = -ry.d + d;
		float ddn_dx = do_dx.dot(n) - d.dot(dn_dx);
		float ddn_dy = do_dy.dot(n) - d.dot(dn_dy);
		refl.rx.d = refl.d - do_dx + 2 * Vector{-d.dot(n) * dn_dx + ddn_dx * n};
		refl.ry.d = refl.d - do_dy + 2 * Vector{-d.dot(n) * dn_dy + ddn_dy * n};
		refl.rx.d = refl.rx.d.normalized();
		refl.ry.d = refl.ry.d.normalized();
	}
	return refl;
}

