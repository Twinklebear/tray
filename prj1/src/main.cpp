#include <iostream>
#include <string>
#include <tinyxml2.h>
#include "args.h"
#include "load_scene.h"

int main(int argc, char **argv){
	if (!flag(argv, argv + argc, "-f")){
		std::cerr << "Error: No scene file passed, aborting\n";
		return 1;
	}
	std::string scene_file = get_param<std::string>(argv, argv + argc, "-f");
	std::cout << "Scene file: " << scene_file << std::endl;
	load_scene(scene_file);
	return 0;
}

