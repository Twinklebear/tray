#include <iostream>
#include <string>
#include <tinyxml2.h>
#include "args.h"
#include "load_scene.h"
#include "render/render_target.h"
#include "render/camera.h"

int main(int argc, char **argv){
	Camera cam{Transform::look_at(Point{0, 0, 0}, Point{0, 20, 0}, Vector{0, 0, 1}),
		std::array<float, 4>{-2, 2, -2, 2}, 45, 4, 4};
	Ray r = cam.generate_ray(0, 0);
	std::cout << "Ray through (0, 0) = " << r << std::endl;

	return 0;
}

