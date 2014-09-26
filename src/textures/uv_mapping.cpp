#include "linalg/vector.h"
#include "textures/uv_mapping.h"

UVMapping::UVMapping(const Vector &scale, const Vector &translate)
	: scale(scale), translate(translate)
{}
TextureSample UVMapping::map(const DifferentialGeometry &dg) const {
	TextureSample sample;
	sample.s = dg.u * scale.x + translate.x;
	sample.t = dg.v * scale.y + translate.y;
	sample.ds_dx = scale.x * dg.du_dx;
	sample.dt_dx = scale.y * dg.dv_dx;
	sample.ds_dy = scale.x * dg.du_dy;
	sample.dt_dy = scale.y * dg.dv_dy;
	return sample;
}

