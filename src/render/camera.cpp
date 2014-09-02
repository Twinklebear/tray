#include <limits>
#include <array>
#include "linalg/vector.h"
#include "linalg/point.h"
#include "linalg/ray.h"
#include "linalg/transform.h"
#include "render/camera.h"

Camera::Camera(const Transform &cam_world, float fov, int xres, int yres) : cam_world(cam_world){
	//Compute x & y dimensions of image plane in screen space, in this
	//space the image size is normalized so that the shorter axis has
	//a half length of 1, while the longer has a half length of aspect ratio
	float aspect_ratio = static_cast<float>(xres) / yres;
	float screen[2];
	if (aspect_ratio > 1){
		screen[0] = aspect_ratio;
		screen[1] = 1;
	}
	else {
		screen[0] = 1;
		screen[1] = 1 / aspect_ratio;
	}
	Transform screen_raster = Transform::scale(xres, yres, 1)
		* Transform::scale(1.f / (2 * screen[0]), 1.f / (2 * screen[1]), 1)
		* Transform::translate(Vector{screen[0], screen[1], 0});
	Transform raster_screen = screen_raster.inverse();
	Transform cam_screen = Transform::perspective(fov, 1, 1000);
	raster_cam = cam_screen.inverse() * raster_screen;
	std::cout << "cam world: " << cam_world << std::endl;
}
Ray Camera::generate_ray(float x, float y) const {
	//Take the raster space position -> camera space
	Point px_pos{x, y, 0};
	raster_cam(px_pos, px_pos);
	//Shoot ray from origin (camera pos) through the point
	Ray ray{Point{0, 0, 0}, Vector{px_pos}.normalized()};
	//Transform the ray to world space so it can be cast through the scene
	cam_world(ray, ray);
	return ray;
}

