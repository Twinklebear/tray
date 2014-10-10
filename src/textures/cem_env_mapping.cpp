#include "linalg/point.h"
#include "linalg/transform.h"
#include "textures/texture_mapping.h"
#include "textures/cem_env_mapping.h"

CemEnvMapping::CemEnvMapping(const Transform &transform) : transform(transform){}
TextureSample CemEnvMapping::map(const DifferentialGeometry &dg) const {
	TextureSample sample;
	Vector d = Vector{transform(dg.point)}.normalized();
	//Cem's wierd mapping
	float z = std::asin(-d.z) / PI + 0.5f;
	float x = d.x / (std::abs(d.x) + std::abs(d.y));
	float y = d.y / (std::abs(d.x) + std::abs(d.y));
	Point sp = Point{0.5, 0.5, 0} + z * (x * Point{0.5, 0.5, 0} + y * Point{-0.5, 0.5, 0});
	sample.s = sp.x;
	sample.t = sp.y;
	return sample;
}

