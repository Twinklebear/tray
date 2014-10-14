#include <vector>
#include "linalg/util.h"
#include "render/color.h"
#include "textures/texture_mapping.h"
#include "textures/mipmap.h"

MipMap::MipLevel::MipLevel(int width, int height, const std::vector<uint8_t> &texels)
	: width(width), height(height), texels(texels)
{}

MipMap::MipMap(){}
MipMap::MipMap(const std::vector<uint8_t> &texels, int width, int height, int ncomp, WRAP_MODE wrap_mode)
	: wrap_mode(wrap_mode), width(width), height(height), ncomp(ncomp)
{
	//TODO: Handle non-pow2 image dimensions
	int n_levels = 1 + static_cast<int>(log_2(std::max(width, height)));
	pyramid.reserve(n_levels);
	pyramid.emplace_back(width, height, texels);
	for (int i = 1; i < n_levels; ++i){
		int w = std::max(1, pyramid[i - 1].width / 2);
		int h = std::max(1, pyramid[i - 1].height / 2);
		pyramid.emplace_back(w, h);
		std::vector<uint8_t> &img = pyramid.back().texels;
		img.resize(w * h * ncomp);
		for (int t = 0; t < h; ++t){
			for (int s = 0; s < w; ++s){
				Colorf cf = 0.25 * (texel(i - 1, 2 * s, 2 * t) + texel(i - 1, 2 * s + 1, 2 * t)
					+ texel(i - 1, 2 * s, 2 * t + 1) + texel(i - 1, 2 * s + 1, 2 * t + 1));
				cf.normalize();
				Color24 c{cf};
				//Write the number of components that we have only
				for (int j = 0; j < ncomp; ++j){
					img[t * w * ncomp + s * ncomp + j] = c[j];
				}
			}
		}
	}
}
Colorf MipMap::texel(int level, int s, int t) const {
	if (width == 0){
		return Colorf{0};
	}
	const MipLevel &mlvl = pyramid[level];
	switch (wrap_mode){
		case WRAP_MODE::REPEAT:
			s = std::abs(s) % mlvl.width;
			t = std::abs(t) % mlvl.height;
			break;
		case WRAP_MODE::CLAMP:
			s = clamp(s, 0, mlvl.width - 1);
			t = clamp(t, 0, mlvl.height - 1);
			break;
		case WRAP_MODE::BLACK:
			if (s < 0 || s >= mlvl.width || t < 0 || t >= mlvl.height){
				return Colorf{0};
			}
	}
	static const float inv = 1.f / 255.f;
	switch (ncomp){
		case 1:
			return Colorf{mlvl.texels[t * mlvl.width + s] * inv};
		case 2:
			return Colorf{mlvl.texels[t * mlvl.width * ncomp + s * ncomp] * inv,
				mlvl.texels[t * mlvl.width * ncomp + s * ncomp + 1] * inv, 0};
		default:
			return Colorf{mlvl.texels[t * mlvl.width * ncomp + s * ncomp] * inv,
				mlvl.texels[t * mlvl.width * ncomp + s * ncomp + 1] * inv,
				mlvl.texels[t * mlvl.width * ncomp + s * ncomp + 2] * inv};
	}
}
Colorf MipMap::sample(const TextureSample &samp) const {
	return sample_trilinear(samp, 2 * std::max(std::max(std::abs(samp.ds_dx), std::abs(samp.dt_dx)),
		std::max(std::abs(samp.ds_dy), std::abs(samp.dt_dy))));
}
Colorf MipMap::sample_trilinear(const TextureSample &samp, float w) const {
	float level = pyramid.size() - 1 + std::log2(std::max(w, 1e-8f));
	if (level < 0){
		return triangle_filter(0, samp.s, samp.t);
	}
	if (level >= pyramid.size() - 1){
		return triangle_filter(pyramid.size() - 1, 0, 0);
	}
	//Find integer coord of the level and the percentage we're in each
	int lvl = static_cast<int>(level);
	float a = level - lvl;
	return (1 - a) * triangle_filter(lvl, samp.s, samp.t)
		+ a * triangle_filter(lvl + 1, samp.s, samp.t);
}
Colorf MipMap::triangle_filter(int lvl, float s, float t) const {
	lvl = clamp(lvl, 0, static_cast<int>(pyramid.size() - 1));
	s = s * pyramid[lvl].width - 0.5f;
	t = t * pyramid[lvl].height - 0.5f;
	int si = static_cast<int>(s);
	int ti = static_cast<int>(t);
	float ds = s - si;
	float dt = t - ti;
	return (1 - ds) * (1 - dt) * texel(lvl, si, ti)
		+ (1 - ds) * dt * texel(lvl, si, ti + 1)
		+ ds * (1 - dt) * texel(lvl, si + 1, ti)
		+ ds * dt * texel(lvl, si + 1, ti + 1);
}

