#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <tinyxml2.h>
#include "args.h"
#include "load_scene.h"
#include "render/render_target.h"
#include "driver.h"

/*
 * Run intersection tests on all the children of the node
 * returns true if any child was hit
 */
bool intersect_children(Node &node, Ray &ray);

int main(int argc, char **argv){
	if (!flag(argv, argv + argc, "-f")){
		std::cerr << "Error: No scene file passed\n";
		return 1;
	}
	if (!flag(argv, argv + argc, "-o")){
		std::cerr << "Error: No output filename passed\n";
		return 1;
	}
	int n_threads = 1;
	if (flag(argv, argv + argc, "-n")){
		n_threads = get_param<int>(argv, argv + argc, "-n");
		if (n_threads > 1 && n_threads % 2 != 0){
			std::cerr << "Warning: num threads not even, increasing thread count by 1\n";
			++n_threads;
		}
	}
	std::string scene_file = get_param<std::string>(argv, argv + argc, "-f");
	std::string out_file = get_param<std::string>(argv, argv + argc, "-o");
	std::cout << "scene file: " << scene_file << ", output: " << out_file << std::endl;
	Scene scene = load_scene(scene_file);
	Driver driver{scene, n_threads};

	auto start = std::chrono::high_resolution_clock::now();
	//While the driver is rendering defer priority to the worker threads
	driver.render();
	while (!driver.done()){
		std::this_thread::yield();
		auto end = std::chrono::high_resolution_clock::now();
		auto elapsed = end - start;
		if (std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() > 32){
			std::cout << "Cancelling" << std::endl;
			driver.cancel();
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = end - start;
	std::cout << "Rendering took: "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count()
		<< "ms\n";

	RenderTarget &target = scene.get_render_target();
	target.save_image(out_file + ".ppm");
	target.save_depth(out_file + ".pgm");
	return 0;
}

