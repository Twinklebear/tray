#ifndef PBR_MATERIAL_H
#define PBR_MATERIAL_H

#include <vector>
#include "geometry/differential_geometry.h"
#include "cache.h"
#include "bsdf.h"

/*
 * TODO: Replace Material with this
 */
class PBRMaterial {
public:
	/*
	 * Get the BSDF to compute the shading for the material at this
	 * piece of geometry
	 */
	virtual BSDF get_bsdf(const DifferentialGeometry &dg) const = 0;
};

typedef Cache<PBRMaterial> PBRMaterialCache;

#endif

