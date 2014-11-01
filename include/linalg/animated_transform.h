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
	Transform start_transform, end_transform;
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
	/*
	 * Compute the interpolated transform at some point
	 */
	Transform interpolate(float t) const;
	void interpolate(float t, Transform &transform) const;
	/*
	 * Compute the bounds the BBox covers when animated with this transformation
	 */
	BBox motion_bound(const BBox &b) const;
	Point operator()(float t, const Point &p) const;
	void operator()(float t, const Point &p, Point &out) const;
	Vector operator()(float t, const Vector &v) const;
	void operator()(float t, const Vector &v, Vector &out) const;
	Ray operator()(const Ray &r) const;
	void operator()(const Ray &r, Ray &out) const;
	RayDifferential operator()(const RayDifferential &r) const;
	void operator()(const RayDifferential &r, RayDifferential &out) const;

private:
	/*
	 * Decompose a transformation into its translation, rotation
	 * and scaling components as descibed by Shoemake and Duff & PBR
	 */
	static void decompose(const Matrix4 &m, Vector &trans, Quaternion &rot,
		Matrix4 &scale);
};

#endif

