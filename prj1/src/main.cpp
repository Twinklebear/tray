#include <iostream>
#include <string>
#include <chrono>
#include <tinyxml2.h>
#include "args.h"
#include "load_scene.h"
#include "render/render_target.h"
#include "render/camera.h"
#include "geometry/sphere.h"
#include "geometry/geometry_cache.h"

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

	Node &root = scene.get_root();
	RenderTarget &target = scene.get_render_target();
	Camera &camera = scene.get_camera();

	//Compute the number of columns and rows to partition the image into so
	//we can assign disjoint portions of it to each thread
	//Should warn if the cols & rows don't evenly divide the space,
	//eg if rect dimensions aren't integers
	int n_cols = std::ceil(std::sqrt(n_threads));
	int n_rows = std::ceil(n_threads / static_cast<float>(n_cols));
	std::cout << "cols: " << n_cols << " rows: " << n_rows
		<< "\nEach rect is " << target.get_width() / static_cast<float>(n_cols)
		<< " x " << target.get_height() / static_cast<float>(n_rows)
		<< std::endl;

	auto start = std::chrono::high_resolution_clock::now();
	for (size_t y = 0; y < target.get_height(); ++y){
		for (size_t x = 0; x < target.get_width(); ++x){
			Ray ray = camera.generate_ray(x + 0.5, y + 0.5);
			if (intersect_children(root, ray)){
				target.write_pixel(x, y, Color{255, 255, 255});
				target.write_depth(x, y, ray.max_t);
			}
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = end - start;
	std::cout << "Rendering took: "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count()
		<< "ms\n";

	target.save_image(out_file + ".ppm");
	target.save_depth(out_file + ".pgm");
	return 0;
}
bool intersect_children(Node &node, Ray &ray){
	bool hit = false;
	//Transform the ray into this nodes space
	Ray node_space = ray;
	node.get_transform().inverse()(ray, node_space);
	for (auto &c : node.get_children()){
		hit = intersect_children(*c, node_space) || hit;
	}
	//Now test this node
	if (node.get_geometry()){
		hit = node.get_geometry()->intersect(node_space) || hit;
	}
	ray.max_t = node_space.max_t;
	return hit;
}

