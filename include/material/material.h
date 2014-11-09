#ifndef MATERIAL_H
#define MATERIAL_H

#include <vector>
#include "geometry/differential_geometry.h"
#include "cache.h"
#include "memory_pool.h"
#include "bsdf.h"

/*
 * Base interface for materials to implement, should return the BSDF that
 * describes the material properties at the point
 */
class Material {
public:
	/*
	 * Get the BSDF to compute the shading for the material at this
	 * piece of geometry. Allocation of the BxDFs and BSDF will be done in the
	 * memory pool passed
	 */
	virtual BSDF* get_bsdf(const DifferentialGeometry &dg, MemoryPool &pool) const = 0;
};

typedef Cache<Material> MaterialCache;

#endif

