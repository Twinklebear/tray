#include <cstdio>
#include <iostream>
#include <vector>
#include <future>
#include <algorithm>
#include "geometry/tri_mesh.h"
#include "loaders/async_loader.h"
#include "mesh_preprocess.h"

void batch_process(char **argv, int argc){
	auto files = std::find(argv, argv + argc, std::string{"-pmesh"}) + 1;
	AsyncLoader loader;
	for (char **f = files; f < argv + argc; ++f){
		std::string mesh{*f};
		loader.run_task("process mesh: " + mesh, process_wobj, mesh);
	}
	//Wait for all the tasks to finish
	loader.wait();
}
bool process_wobj(const std::string &file){
	std::cout << "Processing mesh " << file << std::endl;
	TriMesh mesh{file, true};
	if (mesh.vert_indices.empty()){
		std::cout << "Error: process_wobj failed to load model " << file << std::endl;
		return false;
	}
	std::string file_out = file.substr(0, file.rfind("obj")) + "bobj";
	std::cout << "Writing binary mesh to " << file_out << std::endl;

	std::FILE *fout = std::fopen(file_out.c_str(), "wb");
	uint32_t nverts = mesh.vertices.size();
	uint32_t ntris = mesh.tris.size();
	std::fwrite(&nverts, sizeof(uint32_t), 1, fout);
	std::fwrite(&ntris, sizeof(uint32_t), 1, fout);
	std::fwrite(mesh.vertices.data(), sizeof(Point), nverts, fout);
	std::fwrite(mesh.texcoords.data(), sizeof(Point), nverts, fout);
	std::fwrite(mesh.normals.data(), sizeof(Normal), nverts, fout);
	std::fwrite(mesh.vert_indices.data(), sizeof(int), 3 * ntris, fout);
	std::fclose(fout);
	return true;
}

