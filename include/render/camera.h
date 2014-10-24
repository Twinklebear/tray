#ifndef CAMERA_H
#define CAMERA_H

#include <array>
#include "linalg/ray.h"
#include "linalg/transform.h"
#include "samplers/sampler.h"

class Camera {
	//Transform to go from camera to world space, eg. camera look at matrix
	Transform cam_world;
	//Transforms from raster to camera space
	Transform raster_cam;
	//dx and dy for pixels, for computing ray differentials
	Vector dx, dy;

public:
	/*
	 * Create the camera at some position in the world. Default is looking down
	 * the z axis but this can be changed with the cam_world transformation
	 * xres & yres correspond to the render target resolution
	 */
	Camera(const Transform &cam_world, float fov, int xres, int yres);
	/*
	 * Generate a ray to sample the scene through x,y. Pixel coordinates should be
	 * in raster space
	 */
	Ray generate_ray(const Sample &sample) const;
	/*
	 * Generate a ray with differentials to sample the scene through x,y.
	 * The differentials will be +1 in x and y, pixel coords passed should
	 * be in raster space
	 */
	RayDifferential generate_raydifferential(const Sample &sample) const;
};

#endif

