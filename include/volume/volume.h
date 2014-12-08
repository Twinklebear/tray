#ifndef VOLUME_H
#define VOLUME_H

#include "cache.h"
#include "geometry/bbox.h"
#include "film/color.h"
#include "linalg/util.h"
#include "linalg/ray.h"

/*
 * Various common phase functions provided for the volumes to use
 * phase functions take the incident direction as w_i and the
 * outgoing direction as w_o
 * See: PBR
 */
constexpr float phase_isotropic(){
	return 1.f / (4 * PI);
}
float phase_rayleigh(const Vector &w_i, const Vector &w_o);
float phase_mie_hazy(const Vector &w_i, const Vector &w_o);
float phase_mie_murky(const Vector &w_i, const Vector &w_o);
float phase_henyey_greenstein(const Vector &w_i, const Vector &w_o, float g);
float phase_schlick(const Vector &w_i, const Vector &w_o, float g);

/*
 * Interface that describes a volume occupying some region in the scene
 * based on PBRT's VolumeRegion
 */
class Volume {
public:
	/*
	 * Get the object-space bounds for the volume region
	 */
	virtual BBox bound() const = 0;
	/*
	 * Test if the ray intersects the volume region, returning the min/max t
	 * values that the ray overlapped the region in
	 */
	virtual bool intersect(const Ray &ray, std::array<float, 2> &t) const = 0;
	/*
	 * Get the absorption coefficient of the volume at some point
	 * along the direction passed
	 */
	virtual Colorf absorption(const Point &p, const Vector &v) const = 0;
	/*
	 * Get the scattering coefficient of the volume at some point
	 * along the direction passed
	 */
	virtual Colorf scattering(const Point &p, const Vector &v) const = 0;
	/*
	 * Get the attenuation coefficient of the volume at some poiint
	 * along the direciton passed
	 */
	virtual Colorf attenuation(const Point &p, const Vector &v) const;
	/*
	 * Get the emission property of the volume region at some point
	 * along the direction passed
	 */
	virtual Colorf emission(const Point &p, const Vector &v) const = 0;
	/*
	 * Get the optical thickness of the region along the ray on the line
	 * segment from min_t to max_t
	 * For volumes that use Monte Carlo integration to compute this value step
	 * and offset are used to control how to step along the segment to estimate
	 * the integral
	 */
	virtual Colorf optical_thickness(const Ray &ray, float step = 1, float offset = 0.5) const = 0;
	/*
	 * Get the phase function value for the volume at the some point
	 * for a ray incident along w_i and exiting along w_o
	 */
	virtual float phase(const Point &p, const Vector &w_i, const Vector &w_o) const = 0;
};

typedef Cache<Volume> VolumeCache;

#endif

