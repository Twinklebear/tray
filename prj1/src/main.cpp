#include <iostream>
#include <string>
#include <tinyxml2.h>
#include "args.h"
#include "load_scene.h"
#include "render/render_target.h"

int main(int argc, char **argv){
	if (!flag(argv, argv + argc, "-f")){
		std::cerr << "Error: No scene file passed, aborting\n";
		return 1;
	}
	if (!flag(argv, argv + argc, "-o")){
		std::cerr << "Error: No output file name passed, aborting\n";
		return 1;
	}
	std::string scene_file = get_param<std::string>(argv, argv + argc, "-f");
	std::cout << "Scene file: " << scene_file << std::endl;
	load_scene(scene_file);

	std::string out_file = get_param<std::string>(argv, argv + argc, "-o");
	RenderTarget target(2, 2);
	target.write_pixel(0, 0, Color{255, 0, 0});
	target.write_pixel(1, 0, Color{0, 255, 0});
	target.write_pixel(0, 1, Color{0, 0, 255});
	target.write_pixel(1, 1, Color{255, 255, 255});

	target.write_depth(1, 1, 100);
	target.write_depth(1, 0, 50);
	target.write_depth(0, 1, 25);
	target.write_depth(0, 0, 0);

	target.save_image(out_file + ".ppm");
	target.save_depth(out_file + ".pgm");

	return 0;
}

