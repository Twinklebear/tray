#include "material/bsdf.h"
#include "material/scaled_bxdf.h"
#include "material/mix_material.h"

MixMaterial::MixMaterial(const Material *mat_a, const Material *mat_b, const Colorf &scale)
	: mat_a(mat_a), mat_b(mat_b), scale(scale)
{}
BSDF* MixMaterial::get_bsdf(const DifferentialGeometry &dg, MemoryPool &pool) const {
	//Get the BSDFs from the materials we're mixing and scale their component BxDFs to
	//create a new BSDF that combines the two
	BSDF *bsdf = pool.alloc<BSDF>(dg);
	BSDF *a = mat_a->get_bsdf(dg, pool);
	for (int i = 0; i < a->num_bxdfs(); ++i){
		bsdf->add(pool.alloc<ScaledBxDF>((*a)[i], scale));
	}
	BSDF *b = mat_b->get_bsdf(dg, pool);
	Colorf b_scale = Colorf{1} - scale;
	for (int i = 0; i < b->num_bxdfs(); ++i){
		bsdf->add(pool.alloc<ScaledBxDF>((*b)[i], b_scale));
	}
	return bsdf;
}

