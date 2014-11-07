#include <iostream>
#include "lights/pbr_light.h"

PBRLight::PBRLight(const Transform &to_world, int n_samples)
	: to_world(to_world), to_light(to_world.inverse()),	n_samples(n_samples)
{
	if (to_light.has_scale()){
		std::cout << "WARNING: The renderer assumes that light transformations"
			<< " don't have any scaling factors in them, this could be a problem!\n";
	}
}

