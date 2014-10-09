#include <memory>
#include <cmath>
#include <array>
#include "textures/texture.h"
#include "textures/texture_mapping.h"
#include "textures/checkerboard_texture.h"

CheckerboardTexture::CheckerboardTexture(const Colorf &a, const Colorf &b,
	std::unique_ptr<TextureMapping> mapping)
	: mapping(std::move(mapping)), a(a), b(b)
{}
Colorf CheckerboardTexture::sample(const DifferentialGeometry &dg) const {
	TextureSample sample = mapping->map(dg);
	//Cem considers each check to be 0.5 wide instead of 1 unit wide so scale to match
	sample.s *= 2;
	sample.t *= 2;
	sample.ds_dx *= 2;
	sample.ds_dy *= 2;
	sample.dt_dx *= 2;
	sample.dt_dy *= 2;
	//Filter the checkerboard texture by computing an AABB about (s, t)
	//hs and ht are the half vectors of this box in s and t respectively
	float hs = std::max(std::abs(sample.ds_dx), std::abs(sample.ds_dy));
	float ht = std::max(std::abs(sample.dt_dx), std::abs(sample.dt_dy));
	std::array<float, 2> s = {sample.s - hs, sample.s + hs};
	std::array<float, 2> t = {sample.t - ht, sample.t + ht};

	//Check if the AABB is entirely in one check
	if (static_cast<int>(s[0]) == static_cast<int>(s[1])
		&& static_cast<int>(t[0]) == static_cast<int>(t[1]))
	{
		if ((static_cast<int>(sample.s) + static_cast<int>(sample.t)) % 2 == 0){
			return a;
		}
		return b;
	}
	//Compute the integrals of the step function in s and t to find out
	//how much area of each check we overlap
	float s_int = (step_integral(s[1]) - step_integral(s[0])) / (2 * hs);
	float t_int = (step_integral(t[1]) - step_integral(t[0])) / (2 * ht);
	float area_b = s_int + t_int - 2 * s_int * t_int;
	if (hs > 1 || ht > 1){
		area_b = 0.5;
	}
	//Interpolate the checks based on how much area each occupies in our sample
	return (1 - area_b) * a + area_b * b;
}
float CheckerboardTexture::step_integral(float x) const {
	float fx = std::floor(x / 2);
	return fx + 2.f * std::max(x / 2.f - fx - 0.5f, 0.f);
}

