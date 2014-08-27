#include <iostream>
#include <string>
#include <tinyxml2.h>
#include "args.h"
#include "load_scene.h"
#include "render/render_target.h"
#include "render/camera.h"
#include "geometry/sphere.h"
#include "geometry/geometry_cache.h"

int main(int argc, char **argv){
	if (flag(argv, argv + argc, "-f")){
		std::string scene_file = get_param<std::string>(argv, argv + argc, "-f");
		std::cout << "scene file: " << scene_file << std::endl;
		load_scene(scene_file);
	}
	Camera cam{Transform::look_at(Point{0, 0, 0}, Point{0, 20, 0}, Vector{0, 0, 1}),
		std::array<float, 4>{-2, 2, -2, 2}, 45, 4, 4};
	Ray r = cam.generate_ray(0, 0);
	std::cout << "Ray through (0, 0) = " << r << std::endl;

	GeometryCache cache;
	cache.add("sphere", std::unique_ptr<Geometry>{new Sphere{Point{0, 0, 0}, 1}});
	cache.get("sphere")->intersect(r);

	return 0;
}

