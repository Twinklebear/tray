#ifndef GRID_VOLUME_H
#define GRID_VOLUME_H

#include <string>
#include <vector>
#include "varying_density_volume.h"

/*
 * Defines a volume where the density values are stored in some
 * 3D grid. Currently only gridvolume format encoding 1 from Mitsuba is supported
 * See the grid-based volume data source section of the Mitsuba doc for the file format:
 * http://www.mitsuba-renderer.org/releases/current/documentation.pdf
 *
 * Wenzel Jakob's fluid simulator is quick to get running and outputs
 * files in the supported format:
 * http://www.mitsuba-renderer.org/misc.html
 */
class GridVolume : public VaryingDensityVolume {
	float density_scale;
	//Number of cells along x/y/z
	uint32_t n_x, n_y, n_z;
	BBox region;
	std::vector<float> grid;

public:
	/*
	 * Create the grid volume specifying the properties of the volume and
	 * the Mitsuba vol file to load the density grid from and an optional
	 * extra scaling to apply to the density values
	 */
	GridVolume(const Colorf &sig_a, const Colorf &sig_s, const Colorf &emit,
		float phase_asymmetry, const std::string &vol_file, float density_scale = 1);
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

private:
	/*
	 * Utility to easily lookup the density at some location in the grid
	 */
	float grid_density(uint32_t x, uint32_t y, uint32_t z) const;
	/*
	 * Load the volume data from the vol file
	 * returns true if successful
	 */
	bool load_vol_file(const std::string &vol_file);
};

#endif

