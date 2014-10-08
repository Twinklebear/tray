#include "linalg/transform.h"
#include "linalg/vector.h"
#include "textures/uv_mapping.h"

UVMapping::UVMapping(const Transform &transform) : transform(transform){}
TextureSample UVMapping::map(const DifferentialGeometry &dg) const {
	TextureSample sample;
	Point p{dg.u, dg.v, 0};
	transform(p, p);
	sample.s = p.x;
	sample.t = p.y;
	Vector d{dg.du_dx, dg.dv_dx, 0};
	transform(d, d);
	sample.ds_dx = d.x;
	sample.dt_dx = d.y;
	d = Vector{dg.du_dy, dg.dv_dy, 0};
	transform(d, d);
	sample.ds_dy = d.x;
	sample.dt_dy = d.y;
	return sample;
}

