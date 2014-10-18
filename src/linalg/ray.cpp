#include "linalg/vector.h"
#include "linalg/point.h"
#include "geometry/differential_geometry.h"
#include "linalg/ray.h"

RayDifferential RayDifferential::reflect(const DifferentialGeometry &dg) const {
	Vector n{dg.normal.normalized()};
	Vector dir = d - 2 * n.dot(d) * n;
	RayDifferential refl{dg.point, dir.normalized(), *this, 0.001};
	if (has_differentials()){
		refl.rx = Ray{refl.o + dg.dp_dx, refl.d, *this, 0.001};
		refl.ry = Ray{refl.o + dg.dp_dy, refl.d, *this, 0.001};
		//We compute dn_dx and dn_dy as described in PBR since we're following their differential
		//geometry method, the rest of the computation is directly from Igehy's paper
		auto dn_dx = Vector{dg.dn_du * dg.du_dx + dg.dn_dv * dg.dv_dx};
		auto dn_dy = Vector{dg.dn_du * dg.du_dy + dg.dn_dv * dg.dv_dy};
		auto dd_dx = -rx.d + d;
		auto dd_dy = -ry.d + d;
		float ddn_dx = dd_dx.dot(n) - d.dot(dn_dx);
		float ddn_dy = dd_dy.dot(n) - d.dot(dn_dy);
		refl.rx.d = refl.d - dd_dx + 2 * Vector{-d.dot(n) * dn_dx + ddn_dx * n};
		refl.ry.d = refl.d - dd_dy + 2 * Vector{-d.dot(n) * dn_dy + ddn_dy * n};
		refl.rx.d = refl.rx.d.normalized();
		refl.ry.d = refl.ry.d.normalized();
	}
	return refl;
}
RayDifferential RayDifferential::refract(const DifferentialGeometry &dg, const Vector &n, float eta) const {
	float c = -n.dot(d);
	float root = std::sqrt(1 - eta * eta * (1 - c * c));
	Vector refr_dir = eta * d + (eta * c - root) * n;
	RayDifferential refr{dg.point, refr_dir.normalized(), *this, 0.001};
	if (has_differentials()){
		refr.rx = Ray{refr.o + dg.dp_dx, refr.d, *this, 0.001};
		refr.ry = Ray{refr.o + dg.dp_dy, refr.d, *this, 0.001};
		//We compute dn_dx and dn_dy as described in PBR since we're following their differential
		//geometry method, the rest of the computation is directly from Igehy's paper
		auto dn_dx = Vector{dg.dn_du * dg.du_dx + dg.dn_dv * dg.dv_dx};
		auto dn_dy = Vector{dg.dn_du * dg.du_dy + dg.dn_dv * dg.dv_dy};
		auto dd_dx = -rx.d + d;
		auto dd_dy = -ry.d + d;
		float ddn_dx = dd_dx.dot(n) - d.dot(dn_dx);
		float ddn_dy = dd_dy.dot(n) - d.dot(dn_dy);
		float mu = eta * d.dot(n) - refr.d.dot(n);
		float dmu_dx = (eta - eta * eta * d.dot(n) / refr.d.dot(n)) * ddn_dx;
		float dmu_dy = (eta - eta * eta * d.dot(n) / refr.d.dot(n)) * ddn_dy;
		refr.rx.d = refr.rx.d + eta * dd_dx - Vector{mu * dn_dx + dmu_dx * n};
		refr.ry.d = refr.ry.d + eta * dd_dy - Vector{mu * dn_dy + dmu_dy * n};
		refr.rx.d = refr.rx.d.normalized();
		refr.ry.d = refr.ry.d.normalized();
	}
	return refr;
}

