#ifndef UV_MAPPING_H
#define UV_MAPPING_H

#include "linalg/transform.h"
#include "texture_mapping.h"

class UVMapping : public TextureMapping {
	Transform transform;

public:
	/*
	 * Create a uv mapping that will scale and translate the texture
	 * coordinates by some amount
	 */
	UVMapping(const Transform &transform);
	/*
	 * Compute the texture sample position and derivatives for the
	 * differential geometry being rendered
	 */
	TextureSample map(const DifferentialGeometry &dg) const override;
};

#endif

