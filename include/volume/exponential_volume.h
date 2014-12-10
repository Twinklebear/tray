#ifndef EXPONENTIAL_VOLUME_H
#define EXPONENTIAL_VOLUME_H

#include "varying_density_volume.h"

/*
 * Defines a volume with density that falls of as an exponential, a*e^{-b*h}
 * where h is the 'height' along the up vector specified
 */
class ExponentialVolume : public VaryingDensityVolume {
	//Parameters for the exponential falloff
	float a, b;
	Vector up;
	BBox region;

public:
	/*
	 * Create the exponential falloff volume specifying both the volume properties
	 * and parameters for the exponential falloff density function
	 */
	ExponentialVolume(const Colorf &sig_a, const Colorf &sig_s, const Colorf &emit, float phase_asymmetry,
		const BBox &region, float a, float b, const Vector &up);
	/*
	 * Get the object-space bounds for the volume region
	 */
	BBox bound() const override;
	/*
	 * Test if the ray intersects the volume region, returning the min/max t
	 * values that the ray overlapped the region in
	 */
	bool intersect(const Ray &ray, std::array<float, 2> &t) const override;
	/*
	 * Return the density of the volume at some point
	 */
	float density(const Point &p) const override;
};

#endif

