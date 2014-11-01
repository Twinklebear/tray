#ifndef ANIMATED_TRANSFORM_H
#define ANIMATED_TRANSFORM_H

#include <array>
#include "linalg/transform.h"
#include "linalg/quaternion.h"

/*
 * Animated transformation based on interpolating two transforms
 * between the given times, based on Shoemake and Duff (1992) and
 * PBR
 */
class AnimatedTransform {
	const float start_time, end_time;
	const bool animated;
	//The decomposed start and end transformations
	std::array<Vector, 2> translation;
	std::array<Quaternion, 2> rotation;
	std::array<Matrix4, 2> scaling;

public:
	/*
	 * Create a transform that interpolates between the start and
	 * end transformations over the start and end times
	 */
	AnimatedTransform(const Transform &start_transform, float start_time,
		const Transform &end_transform, float end_time);
};

#endif

