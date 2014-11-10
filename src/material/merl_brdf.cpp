#include <cassert>
#include <cmath>
#include "material/merl_brdf.h"

MerlBRDF::MerlBRDF(const std::vector<float> &brdf, int n_theta_h, int n_theta_d, int n_phi_d)
	: BxDF(BxDFTYPE(BxDFTYPE::REFLECTION | BxDFTYPE::GLOSSY)), brdf(brdf), n_theta_h(n_theta_h),
	n_theta_d(n_theta_d), n_phi_d(n_phi_d)
{}
Colorf MerlBRDF::operator()(const Vector &w_oi, const Vector &w_ii) const {
	//Find the half-vector and transform into the half angle coordinate system
	Vector w_o = w_oi;
	Vector w_i = w_ii;
	Vector w_h = (w_o + w_i).normalized();
	if (w_h.z < 0){
		w_o = -w_o;
		w_i = -w_i;
		w_h = -w_h;
	}
	if (w_h.length_sqr() == 0){
		return Colorf{0};
	}
	//Directly compute the rows of the matrix performing the rotation of w_h to (0, 0, 1)
	float theta_h = spherical_theta(w_h);
	float cos_phi_h = cos_phi(w_h);
	float sin_phi_h = sin_phi(w_h);
	float cos_theta_h = cos_theta(w_h);
	float sin_theta_h = sin_theta(w_h);
	Vector w_hx{cos_phi_h * cos_theta_h, sin_phi_h * cos_theta_h, -sin_theta_h};
	Vector w_hy{-sin_phi_h, cos_phi_h, 0};
	Vector w_d{w_i.dot(w_hx), w_i.dot(w_hy), w_i.dot(w_h)};
	float theta_d = spherical_theta(w_d);
	float phi_d = spherical_phi(w_d);
	//Wrap phi_d if needed to keep it in range
	if (phi_d > PI){
		phi_d -= PI;
	}
	int theta_h_idx = map_index(std::sqrt(std::max(0.f, theta_h / (PI / 2))), 1, n_theta_h);
	int theta_d_idx = map_index(theta_d, PI / 2, n_theta_d);
	int phi_d_idx = map_index(phi_d, PI, n_phi_d);
	int i = phi_d_idx + n_phi_d * (theta_d_idx + theta_h_idx * n_theta_d);
	assert(i < brdf.size());
	return Colorf{brdf[3 * i], brdf[3 * i + 1], brdf[3 * i + 2]};
}

