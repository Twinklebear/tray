#ifndef VOLUME_NODE_H
#define VOLUME_NODE_H

#include <vector>
#include <memory>
#include <string>
#include "linalg/transform.h"
#include "volume.h"

/*
 * A scene node that holds some volume instead of geometry
 * analogous to the geometry node but for volumes
 */
class VolumeNode : public Volume {
	std::vector<std::shared_ptr<VolumeNode>> children;
	const Volume *volume;
	Transform transform, inv_transform;
	std::string name;

public:
	/*
	 * Create a volume node in the scene, placing some volume in the scene
	 */
	VolumeNode(const Volume *volume, const Transform &t, const std::string &name);
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
	 /*
	  * Get the volume associated with the node, or nullptr if none is attached
	  */
	 const Volume* get_volume() const;
	 /*
	  * Get the transform or inverse of the nodes transform
	  */
	 const Transform& get_transform() const;
	 Transform& get_transform();
	 const Transform& get_inv_transform() const;
	 Transform& get_inv_transform();
};

#endif

