#ifndef PBR_MATERIAL_H
#define PBR_MATERIAL_H

#include <vector>
#include "geometry/differential_geometry.h"
#include "cache.h"
#include "memory_pool.h"
#include "bsdf.h"

/*
 * TODO: Replace Material with this
 */
class PBRMaterial {
public:
	/*
	 * Get the BSDF to compute the shading for the material at this
	 * piece of geometry. Allocation of the BxDFs and BSDF will be done in the
	 * memory pool passed
	 */
	virtual BSDF* get_bsdf(const DifferentialGeometry &dg, MemoryPool &pool) const = 0;
};

typedef Cache<PBRMaterial> PBRMaterialCache;

#endif

