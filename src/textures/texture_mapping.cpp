#include "geometry/differential_geometry.h"
#include "linalg/vector.h"
#include "textures/texture_mapping.h"

TextureSample::TextureSample(float s, float t, float ds_dx, float ds_dy, float dt_dx, float dt_dy)
	: s(s), t(t), ds_dx(ds_dx), ds_dy(ds_dy), dt_dx(dt_dx), dt_dy(dt_dy)
{}

