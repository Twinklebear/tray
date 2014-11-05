#include "lights/pbr_light.h"

PBRLight::PBRLight(const Transform &to_world, int n_samples)
	: to_world(to_world), to_light(to_world.inverse()),	n_samples(n_samples)
{}

