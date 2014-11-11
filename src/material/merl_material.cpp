#include <cassert>
#include <iostream>
#include <cstdio>
#include <array>
#include "material/bsdf.h"
#include "material/merl_brdf.h"
#include "material/merl_material.h"

MerlMaterial::MerlMaterial(const std::string &file) : n_theta_h(90), n_theta_d(90), n_phi_d(180){
	if (!load_brdf(file)){
		std::cout << "MerlMaterial error: could not load " << file << std::endl;
		std::exit(1);
	}
}
BSDF* MerlMaterial::get_bsdf(const DifferentialGeometry &dg, MemoryPool &pool) const {
	BSDF *bsdf = pool.alloc<BSDF>(dg);
	bsdf->add(pool.alloc<MerlBRDF>(brdf, n_theta_h, n_theta_d, n_phi_d));
	return bsdf;
}
bool MerlMaterial::load_brdf(const std::string &file){
	FILE *f = fopen(file.c_str(), "rb");
	if (!f){
		std::cout << "MerlMaterial error: could not open " << file << std::endl;
		return false;
	}
	std::array<int, 3> dims;
	if (fread(dims.data(), sizeof(int), 3, f) != 3){
		std::cout << "MerlMaterial error: file ended unexpectedly" << std::endl;
		fclose(f);
		return false;
	}
	int n_vals = dims[0] * dims[1] * dims[2];
	if (n_vals != n_theta_h * n_theta_d * n_phi_d){
		std::cout << "MerlMaterial error: dimensions in file are incorrect" << std::endl;
		fclose(f);
		return false;
	}
	brdf.resize(3 * n_vals);
	//The values in the file are binary doubles so we need to convert them down
	//and scale them as described in the paper
	uint32_t chunk_size = 2 * n_phi_d;
	std::vector<double> tmp(chunk_size);
	int n_chunks = n_vals / chunk_size;
	assert(n_vals % n_chunks == 0);
	const std::array<float, 3> scaling{1.f / 1500.f, 1.f / 1500.f, 1.66f / 1500.f};
	for (int c = 0; c < 3; ++c){
		int offset = 0;
		for (int i = 0; i < n_chunks; ++i){
			if (fread(tmp.data(), sizeof(double), chunk_size, f) != chunk_size){
				std::cout << "MerlMaterial error: file ended unexpectedly" << std::endl;
				fclose(f);
				return false;
			}
			for (uint32_t j = 0; j < chunk_size; ++j){
				brdf[3 * offset++ + c] = std::max(0.0, tmp[j] * scaling[c]);
			}
		}
	}
	fclose(f);
	return true;
}

