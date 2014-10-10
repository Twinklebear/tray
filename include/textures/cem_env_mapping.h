#ifndef CEM_ENV_MAPPING_H
#define CEM_ENV_MAPPING_H

#include "linalg/point.h"
#include "linalg/transform.h"
#include "texture_mapping.h"

/*
 * Implementation of Cem's not-quite-spherical mapping that is used
 * for the environment mapping in the assignments
 */
class CemEnvMapping : public TextureMapping {
	Transform transform;

public:
	CemEnvMapping(const Transform &transform);
	/*
	 * Compute the texture sample position and derivatives for the
	 * differential geometry being rendered
	 */
	TextureSample map(const DifferentialGeometry &dg) const override;
};

#endif

