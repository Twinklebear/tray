#include <iostream>
#include <string>
#include <chrono>
#include "args.h"
#include "loaders/load_scene.h"
#include "render/render_target.h"
#include "mesh_preprocess.h"
#include "driver.h"

#ifdef BUILD_PREVIEWER
#include "previewer.h"
#endif

const static std::string USAGE =
"Usage for ray tracer:\n\
----------------------------\n\
-f <file>         - Specify the scene file to render\n\
-o <prefix>       - Specify the prefix name for the output files\n\
                    color is written to <prefix>.ppm and depth to\n\
                    <prefix>.pgm\n\
-n <num>          - Optional: specify the number of threads to render with,\n\
                    this number will be rounded up to the nearest even number.\n\
                    A warning will be printed if we can't evenly partition the\n\
                    image into <num> rectangles for rendering. Default is 1.\n\
-bw <num>         - Optional: specify the desired width of blocks to partition\n\
                    the scene into for the threads to work on, should evenly divide\n\
                    the image width. Default is image width.\n\
-bh <num>         - Optional: specify the desired height of blocks to partition\n\
                    the scene into for the threads to work on, should evenly divide\n\
                    the image height. Default is image height.\n\
-d <num>          - Optional: specify the max recursion depth for reflected/refracted rays.\n\
                    Default is 4.\n\
-pmesh [<files>]  - Specify a list of meshes to be run through the\n\
                    the obj -> binary obj (bobj) processor so that they can be\n\
                    loaded faster when doing a render. The renderer will check for\n\
                    bobj files with the same name when trying to load an obj file\n\
                    for rendering.\n"
#ifdef BUILD_PREVIEWER
+ std::string{"-p                - Show a live preview of the image as it's rendered.\n\
                    Rendering performance is not measured in this mode\n"}
#endif
+ std::string{"-h                - Show this help information\n\
----------------------------\n"};

int main(int argc, char **argv){
	if (flag(argv, argv + argc, "-h")){
		std::cout << USAGE;
		return 0;
	}
	if (flag(argv, argv + argc, "-pmesh")){
		batch_process(argv, argc);
		return 0;
	}
	if (!flag(argv, argv + argc, "-f")){
		std::cerr << "Error: No scene file passed\n"
			<< USAGE;
		return 1;
	}
	//if we built the previewer it's valid to not specify a file output but
	//we need some way to output
#ifdef BUILD_PREVIEWER
	if (!flag(argv, argv + argc, "-o") && !flag(argv, argv + argc, "-p")){
		std::cerr << "Error: No output medium specified\n"
			<< USAGE;
		return 1;
	}
#else
	if (!flag(argv, argv + argc, "-o")){
		std::cerr << "Error: No output filename passed\n"
			<< USAGE;
		return 1;
	}
#endif

	int n_threads = 1;
	if (flag(argv, argv + argc, "-n")){
		n_threads = get_param<int>(argv, argv + argc, "-n");
	}
	int bw = -1, bh = -1;
	if (flag(argv, argv + argc, "-bw")){
		bw = get_param<int>(argv, argv + argc, "-bw");
	}
	if (flag(argv, argv + argc, "-bh")){
		bh = get_param<int>(argv, argv + argc, "-bh");
	}
	int depth = 4;
	if (flag(argv, argv + argc, "-d")){
		depth = get_param<int>(argv, argv + argc, "-d");
	}
	std::string scene_file = get_param<std::string>(argv, argv + argc, "-f");
	Scene scene = load_scene(scene_file, depth);
	if (bw == -1){
		bw = scene.get_render_target().get_width();
	}
	if (bh == -1){
		bh = scene.get_render_target().get_height();
	}
	Driver driver{scene, n_threads, bw, bh};

#ifdef BUILD_PREVIEWER
	if (flag(argv, argv + argc, "-p")){
		//The user might abort before rendering completes or we could encounter
		//an SDL/OpenGL error, so don't save the images if the render doesn't complete
		//Also need to validate that file output was turned on if we're running the previewer
		if (render_with_preview(driver) && flag(argv, argv + argc, "-o")){
			std::string out_file = get_param<std::string>(argv, argv + argc, "-o");
			if (out_file.empty()){
				std::cerr << "Error: No output filename specified\n";
				return 1;
			}
			RenderTarget &target = scene.get_render_target();
			target.save_image(out_file + ".ppm");
			target.save_depth(out_file + ".pgm");
		}
		return 0;
	}
	else {
#endif
		auto start = std::chrono::high_resolution_clock::now();
		//While the driver is rendering defer priority to the worker threads
		driver.render();
		while (!driver.done()){
			std::this_thread::yield();
		}
		auto end = std::chrono::high_resolution_clock::now();
		auto elapsed = end - start;
		std::cout << "Rendering took: "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count()
			<< "ms\n";
#ifdef BUILD_PREVIEWER
	}
#endif

	std::string out_file = get_param<std::string>(argv, argv + argc, "-o");
	if (out_file.empty()){
		std::cerr << "Error: No output filename specified\n";
		return 1;
	}
	RenderTarget &target = scene.get_render_target();
	target.save_image(out_file + ".ppm");
	target.save_depth(out_file + ".pgm");
	return 0;
}

