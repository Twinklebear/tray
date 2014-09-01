#ifndef CAMERA_H
#define CAMERA_H

#include <array>
#include "linalg/vector.h"
#include "linalg/point.h"
#include "linalg/transform.h"

class Camera {
	//Transform to go from camera to world space, eg. camera look at matrix
	Transform cam_world;
	//Transforms from raster to camera space
	Transform raster_cam;

public:
	/*
	 * Create the camera at some position in the world. Default is looking down
	 * the z axis but this can be changed with the cam_world transformation
	 * xres & yres correspond to the render target resolution
	 */
	Camera(const Transform &cam_world, float fov, int xres, int yres);
	/*
	 * Generate a ray to sample the pixel at x,y. Pixel coordinates should be
	 * in raster space
	 */
	Ray generate_ray(float x, float y) const;
};

#endif

