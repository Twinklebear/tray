#ifndef HOMOGENEOUS_VOLUME_H
#define HOMOGENEOUS_VOLUME_H

#include "volume.h"

/*
 * A very simple volume, describes a box in space with
 * homogeneous scattering properties throughout
 */
class HomogeneousVolume : public Volume {
	//Scattering, absoprtion, emission coefficients describing the volume
	Colorf sig_a, sig_s, emit;
	float phase_asymmetry;
	//The region that the volume occupies
	BBox region;

public:
	/*
	 * Construct the homgeneous volume specifying its properties and bounds
	 */
	HomogeneousVolume(const Colorf &sig_a, const Colorf &sig_s, const Colorf &emit,
		float phase_asymmetry, const BBox &region);
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
	 * Get the absorption coefficient of the volume at some point
	 * along the direction passed
	 */
	Colorf absorption(const Point &p, const Vector &v) const override;
	/*
	 * Get the scattering coefficient of the volume at some point
	 * along the direction passed
	 */
	Colorf scattering(const Point &p, const Vector &v) const override;
	/*
	 * Get the attenuation coefficient of the volume at some poiint
	 * along the direciton passed
	 */
	Colorf attenuation(const Point &p, const Vector &v) const;
	/*
	 * Get the emission property of the volume region at some point
	 * along the direction passed
	 */
	Colorf emission(const Point &p, const Vector &v) const override;
	/*
	 * Get the optical thickness of the region along the ray on the line
	 * segment from min_t to max_t
	 * For volumes that use Monte Carlo integration to compute this value step
	 * and offset are used to control how to step along the segment to estimate
	 * the integral
	 */
	Colorf optical_thickness(const Ray &ray, float step = 1, float offset = 0.5) const override;
	/*
	 * Get the phase function value for the volume at the some point
	 * for a ray incident along w_i and exiting along w_o
	 */
	float phase(const Point &p, const Vector &w_i, const Vector &w_o) const override;
};

#endif

