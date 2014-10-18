#include <cassert>
#include <vector>
#include "linalg/util.h"
#include "render/color.h"
#include "textures/texture_mapping.h"
#include "textures/mipmap.h"

MipMap::MipLevel::MipLevel(int width, int height, const std::vector<uint8_t> &texels)
	: width(width), height(height), texels(texels)
{}

const int MipMap::WEIGHT_LUT_SIZE;
std::array<float, MipMap::WEIGHT_LUT_SIZE> MipMap::weight_table = {-1};

MipMap::MipMap(){}
MipMap::MipMap(const std::vector<uint8_t> &texels, int width, int height, int ncomp, WRAP_MODE wrap_mode)
	: wrap_mode(wrap_mode), width(width), height(height), ncomp(ncomp)
{
	if (weight_table[0] == -1){
		init_weight_table();
	}
	//We only support power of two textures at the moment
	if (!(width && !(width & (width - 1))) || !(height && !(height & (height - 1)))){
		std::cout << "MipMap Error: Only powers of two are supported at the moment\n";
		assert("Non power of two texture");
	}
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
#ifdef TEX_TRILINEAR
	return sample_trilinear(samp, 2 * std::max(std::max(std::abs(samp.ds_dx), std::abs(samp.dt_dx)),
		std::max(std::abs(samp.ds_dy), std::abs(samp.dt_dy))));
#else
	return sample_ewa(samp, {samp.ds_dx, samp.ds_dy}, {samp.dt_dx, samp.dt_dy});
#endif
}
Colorf MipMap::sample_trilinear(const TextureSample &samp, float w) const {
	float level = pyramid.size() - 1 + std::log2(std::max(w, 1e-8f));
	if (level < 0){
		return triangle_filter(0, samp.s, samp.t);
	}
	if (level >= pyramid.size() - 1){
		return texel(pyramid.size() - 1, 0, 0);
	}
	//Find integer coord of the level and the percentage we're in each
	int lvl = static_cast<int>(level);
	float d = level - lvl;
	return (1 - d) * triangle_filter(lvl, samp.s, samp.t)
		+ d * triangle_filter(lvl + 1, samp.s, samp.t);
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
Colorf MipMap::sample_ewa(const TextureSample &samp, std::array<float, 2> ds, std::array<float, 2> dt) const {
	//Compute the ellipse's major and minor axes, we pick the major axis to be (ds[0], dt[0]) so
	//swap if this wouldn't be the case
	if (ds[0] * ds[0] + dt[0] * dt[0] < ds[1] * ds[1] + dt[1] * dt[1]){
		std::swap(ds[0], ds[1]);
		std::swap(dt[0], dt[1]);
	}
	float major_len = std::sqrt(ds[0] * ds[0] + dt[0] * dt[0]);
	float minor_len = std::sqrt(ds[1] * ds[1] + dt[1] * dt[1]);

	//Clamp the ellipse eccentricity if it's too anisotropic by increasing the minor axis
	static const float max_anisotropy = 8;
	if (minor_len > 0 && minor_len * max_anisotropy < major_len){
		float s = major_len / (minor_len * max_anisotropy);
		ds[1] *= s;
		dt[1] *= s;
		minor_len *= s;
	}
	if (minor_len == 0){
		return triangle_filter(0, samp.s, samp.t);
	}

	float level = pyramid.size() - 1 + std::log2(minor_len);
	if (level >= pyramid.size()){
		return texel(pyramid.size() - 1, 0, 0);
	}
	int lvl = static_cast<int>(level);
	float d = level - lvl;
	return (1 - d) * ewa_filter(lvl, samp.s, samp.t, ds, dt)
		+ d * ewa_filter(lvl + 1, samp.s, samp.t, ds, dt);
}
Colorf MipMap::ewa_filter(int lvl, float s, float t, std::array<float, 2> ds, std::array<float, 2> dt) const {
	lvl = clamp(lvl, 0, static_cast<int>(pyramid.size() - 1));
	s = s * pyramid[lvl].width - 0.5f;
	t = t * pyramid[lvl].height - 0.5f;
	for (size_t i = 0; i < ds.size(); ++i){
		ds[i] *= pyramid[lvl].width;
		dt[i] *= pyramid[lvl].height;
	}
	float a = dt[0] * dt[0] + dt[1] * dt[1] + 1;
	float b = -2 * (ds[0] * dt[0] + ds[1] * dt[1]);
	float c = ds[0] * ds[0] + ds[1] * ds[1] + 1;
	float inv_f = 1 / (a * c - b * b * 0.25f);
	a *= inv_f;
	b *= inv_f;
	c *= inv_f;

	//Compute the ellipse's AABB so we can find which texels to loop over and filter
	float det = -b * b + 4 * a * c;
	float inv_det = 1 / det;
	float u_sqrt = std::sqrt(det * c);
	float v_sqrt = std::sqrt(det * a);
	std::array<int, 2> s_bound{
		static_cast<int>(s - 2 * inv_det * u_sqrt),
		static_cast<int>(s + 2 * inv_det * u_sqrt)
	};
	std::array<int, 2> t_bound{
		static_cast<int>(t - 2 * inv_det * v_sqrt),
		static_cast<int>(t + 2 * inv_det * v_sqrt)
	};

	Colorf color;
	float weight_sum = 0;
	for (int it = t_bound[0]; it <= t_bound[1]; ++it){
		float t_pos = it - t;
		for (int is = s_bound[0]; is <= s_bound[1]; ++is){
			float s_pos = is - s;
			//Find the position of this texel relative to the ellipse and see if it's inside
			float r_sqr = a * s_pos * s_pos + b * s_pos * t_pos + c * t_pos * t_pos;
			if (r_sqr < 1){
				float w = weight_table[std::min(static_cast<int>(r_sqr * WEIGHT_LUT_SIZE), WEIGHT_LUT_SIZE - 1)];
				color += texel(lvl, is, it) * w;
				weight_sum += w;
			}
		}
	}
	return color / weight_sum;
}
void MipMap::init_weight_table(){
	for (int i = 0; i < WEIGHT_LUT_SIZE; ++i){
		float r_sqr = static_cast<float>(i) / (WEIGHT_LUT_SIZE - 1.f);
		weight_table[i] = std::exp(-2.f * r_sqr) - std::exp(-2.f);
	}
}

