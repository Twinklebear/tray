#include <array>
#include "linalg/util.h"
#include "linalg/point.h"
#include "linalg/transform.h"
#include "textures/texture_mapping.h"
#include "textures/spherical_mapping.h"

SphericalMapping::SphericalMapping(const Transform &transform) : transform(transform){}
TextureSample SphericalMapping::map(const DifferentialGeometry &dg) const {
	TextureSample sample;
	sphere_project(dg.point, sample.s, sample.t);
	//Compute differentials using forward differencing
	float sx, tx;
	const static float delta = .1;
	sphere_project(dg.point + delta * dg.dp_dx, sx, tx);
	sample.ds_dx = (sx - sample.s) / delta;
	sample.dt_dx = (tx - sample.t) / delta;
	//Handle wrapping about t
	if (sample.dt_dx > 0.5){
		sample.dt_dx = 1 - sample.dt_dx;
	}
	else if (sample.dt_dx < -0.5){
		sample.dt_dx = -(sample.dt_dx + 1);
	}
	float sy, ty;
	sphere_project(dg.point + delta * dg.dp_dy, sy, ty);
	sample.ds_dy = (sy - sample.s) / delta;
	sample.dt_dy = (sy - sample.t) / delta;
	//Handle wrapping about t
	if (sample.dt_dy > 0.5){
		sample.dt_dy = 1 - sample.dt_dy;
	}
	else if (sample.dt_dy < -0.5){
		sample.dt_dy = -(sample.dt_dy + 1);
	}
	return sample;
}
void SphericalMapping::sphere_project(const Point &p, float &s, float &t) const {
	Vector d = Vector{transform(p)}.normalized();
	float theta = spherical_theta(d);
	float phi = spherical_phi(d);
	s = theta * INV_PI;
	t = phi * INV_TAU;
}

