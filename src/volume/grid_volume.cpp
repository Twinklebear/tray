#include <iostream>
#include <fstream>
#include "linalg/util.h"
#include "volume/grid_volume.h"

GridVolume::GridVolume(const Colorf &sig_a, const Colorf &sig_s, const Colorf &emit,
	float phase_asymmetry, const std::string &vol_file, float density_scale)
	: VaryingDensityVolume(sig_a, sig_s, emit, phase_asymmetry), density_scale(density_scale)
{
	//If we fail to load alert the user and set the region to a degenerate box
	if (!load_vol_file(vol_file)){
		std::cout << "GridVolume error: Could not load volume file " << vol_file << std::endl;
	}
}
BBox GridVolume::bound() const {
	return region;
}
bool GridVolume::intersect(const Ray &ray, std::array<float, 2> &t) const {
	return region.intersect(ray, &t[0], &t[1]);
}
float GridVolume::density(const Point &p) const {
	if (!region.inside(p)){
		return 0;
	}
	//Compute the voxel coordinate of the point
	Vector voxel = region.offset(p);
	voxel.x = voxel.x * n_x - 0.5;
	voxel.y = voxel.y * n_y - 0.5;
	voxel.z = voxel.z * n_z - 0.5;
	//Compute integer coordinates of the voxel
	int v_x = static_cast<int>(voxel.x);
	int v_y = static_cast<int>(voxel.y);
	int v_z = static_cast<int>(voxel.z);
	//Trilinearly interpolate samples from the grid to find the density
	float dx = voxel.x - v_x;
	float dy = voxel.y - v_y;
	float dz = voxel.z - v_z;
	float d00 = lerp(dx, grid_density(v_x, v_y, v_z), grid_density(v_x + 1, v_y, v_z));
	float d10 = lerp(dx, grid_density(v_x, v_y + 1, v_z), grid_density(v_x + 1, v_y + 1, v_z));
	float d01 = lerp(dx, grid_density(v_x, v_y, v_z + 1), grid_density(v_x + 1, v_y, v_z + 1));
	float d11 = lerp(dx, grid_density(v_x, v_y + 1, v_z + 1), grid_density(v_x + 1, v_y + 1, v_z + 1));
	float d0 = lerp(dy, d00, d10);
	float d1 = lerp(dy, d01, d11);
	return lerp(dz, d0, d1);
}
float GridVolume::grid_density(uint32_t x, uint32_t y, uint32_t z) const {
	x = clamp(x, uint32_t{0}, n_x - 1);
	y = clamp(y, uint32_t{0}, n_y - 1);
	z = clamp(z, uint32_t{0}, n_z - 1);
	return grid[(z * n_y + y) * n_x + x] * density_scale;
}
bool GridVolume::load_vol_file(const std::string &vol_file){
	std::ifstream file{vol_file, std::ios::binary};
	char header[3] = {0};
	file.read(&header[0], 3);
	if (header[0] != 'V' && header[1] != 'O' && header[2] != 'L'){
		std::cout << "GridVolume error: File " << vol_file << " is not a Mitsuba volume file\n";
		return false;
	}

	char version = 0;
	file.read(&version, 1);
	if (version != 3){
		std::cout << "GridVolume error: Only volume file version 3 is supported\n";
		return false;
	}

	uint32_t encoding = 0;
	file.read(reinterpret_cast<char*>(&encoding), sizeof(uint32_t));
	if (encoding != 1){
		std::cout << "GridVolume error: Only float32 volumes are supported\n";
		return false;
	}

	file.read(reinterpret_cast<char*>(&n_x), sizeof(uint32_t));
	file.read(reinterpret_cast<char*>(&n_y), sizeof(uint32_t));
	file.read(reinterpret_cast<char*>(&n_z), sizeof(uint32_t));

	uint32_t channels = 0;
	file.read(reinterpret_cast<char*>(&channels), sizeof(uint32_t));
	if (channels != 1){
		std::cout << "GridVolume error: Only single channel volumes are supported\n";
		return false;
	}

	for (int i = 0; i < 3; ++i){
		file.read(reinterpret_cast<char*>(&region.min[i]), sizeof(float));
	}
	for (int i = 0; i < 3; ++i){
		file.read(reinterpret_cast<char*>(&region.max[i]), sizeof(float));
	}
	std::cout << "GridVolume " << vol_file << " object space region of " << region << std::endl;

	grid.resize(n_x * n_y * n_x);
	file.read(reinterpret_cast<char*>(grid.data()), sizeof(float) * grid.size());
	return true;
}

