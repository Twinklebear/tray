#include <limits>
#include <array>
#include "linalg/vector.h"
#include "linalg/point.h"
#include "linalg/ray.h"
#include "linalg/util.h"
#include "linalg/transform.h"
#include "film/camera.h"

Camera::Camera(const Transform &cam_world, float fov, float dof, float focal_dist, int xres, int yres)
	: dof(dof), focal_dist(focal_dist), cam_world(cam_world)
{
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
	dx = raster_cam(Point{1, 0, 0}) - raster_cam(Point{0, 0, 0});
	dy = raster_cam(Point{0, 1, 0}) - raster_cam(Point{0, 0, 0});
}
Ray Camera::generate_ray(const Sample &sample) const {
	//Take the raster space position -> camera space
	Point px_pos{sample.img[0], sample.img[1], 0};
	raster_cam(px_pos, px_pos);
	//Shoot ray from origin (camera pos) through the point
	Ray ray{Point{0}, Vector{px_pos}.normalized()};
	if (dof > 0){
		auto lens = concentric_sample_disk(sample.lens);
		lens[0] *= dof;
		lens[1] *= dof;
		float t = focal_dist / ray.d.z;
		Point focal_pt = ray(t);
		ray.o = Point{lens[0], lens[1], 0.f};
		ray.d = (focal_pt - ray.o).normalized();
	}
	//Transform the ray to world space so it can be cast through the scene
	cam_world(ray, ray);
	return ray;
}
RayDifferential Camera::generate_raydifferential(const Sample &sample) const {
	//Take the raster space position -> camera space
	Point px_pos{sample.img[0], sample.img[1], 0};
	raster_cam(px_pos, px_pos);
	//Shoot ray from origin (camera pos) through the point
	RayDifferential ray{Point{0}, Vector{px_pos}.normalized()};
	ray.rx = Ray{ray.o, (Vector{px_pos} + dx).normalized()};
	ray.ry = Ray{ray.o, (Vector{px_pos} + dy).normalized()};
	if (dof > 0){
		auto lens = concentric_sample_disk(sample.lens);
		lens[0] *= dof;
		lens[1] *= dof;
		float t = focal_dist / ray.d.z;
		Point focal_pt = ray(t);
		ray.o = Point{lens[0], lens[1], 0.f};
		ray.d = (focal_pt - ray.o).normalized();

		t = focal_dist / ray.rx.d.z;
		focal_pt = ray.rx(t);
		ray.rx.o = Point{lens[0], lens[1], 0.f};
		ray.rx.d = (focal_pt - ray.rx.o).normalized();

		t = focal_dist / ray.ry.d.z;
		focal_pt = ray.ry(t);
		ray.ry.o = Point{lens[0], lens[1], 0.f};
		ray.ry.d = (focal_pt - ray.ry.o).normalized();
	}
	//Transform the ray to world space so it can be cast through the scene
	cam_world(ray, ray);
	return ray;
}

