#ifndef MIX_MATERIAL_H
#define MIX_MATERIAL_H

#include "material/material.h"

/*
 * Material that mixes the output of two other materials by some color
 * scaling factor
 */
class MixMaterial : public Material {
	const Material *mat_a, *mat_b;
	Colorf scale;

public:
	/*
	 * Specify the two materials to be mixed and the scale level for material a,
	 * material b is scaled by (1 - scale)
	 */
	MixMaterial(const Material *mat_a, const Material *mat_b, const Colorf &scale);
	/*
	 * Get the BSDF to compute the shading for the material at this
	 * piece of geometry. Allocation of the BxDFs and BSDF will be done in the
	 * memory pool passed
	 */
	BSDF* get_bsdf(const DifferentialGeometry &dg, MemoryPool &pool) const override;
};

#endif

