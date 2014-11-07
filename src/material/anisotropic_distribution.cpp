#include <cmath>
#include "material/bxdf.h"
#include "material/anisotropic_distribution.h"

AnisotropicDistribution::AnisotropicDistribution(float exp_x, float exp_y) : exp_x(exp_x), exp_y(exp_y){}
float AnisotropicDistribution::operator()(const Vector &w_h) const {
	float cos_th = std::abs(BxDF::cos_theta(w_h));
	float denom = 1 - cos_th * cos_th;
	if (denom == 0){
		return 0;
	}
	float exponent = (exp_x * w_h.x * w_h.x + exp_y * w_h.y * w_h.y) / denom;
	return std::sqrt((exp_x + 2) * (exp_y + 2)) * INV_TAU * std::pow(cos_th, exponent);
}
void AnisotropicDistribution::sample(const Vector &w_o, Vector &w_i, const std::array<float, 2> &u, float &pdf_val) const {
	//Sample by mapping positions to the first quadrant, sampling it and then mapping back
	float phi, cos_theta;
	if (u[0] < 0.25){
		sample_first_quadrant(u[0], u[1], phi, cos_theta);
	}
	else if (u[0] < 0.5){
		float u_0 = 4 * (0.5 - u[0]);
		sample_first_quadrant(u_0, u[1], phi, cos_theta);
		phi = PI - phi;
	}
	else if (u[0] < 0.75){
		float u_0 = 4 * (u[0] - 0.5);
		sample_first_quadrant(u_0, u[1], phi, cos_theta);
		phi += PI;
	}
	else {
		float u_0 = 4 * (1 - u[0]);
		sample_first_quadrant(u_0, u[1], phi, cos_theta);
		phi = 2 * PI - phi;
	}
	float sin_theta = std::sqrt(std::max(0.f, 1 - cos_theta * cos_theta));
	Vector w_h = spherical_dir(sin_theta, cos_theta, phi);
	if (!BxDF::same_hemisphere(w_o, w_h)){
		w_h = -w_h;
	}
	//The sampled incident direction is the outgoing direction reflected about the half-vector
	w_i = -w_o + 2 * w_o.dot(w_h) * w_h;

	cos_theta = std::abs(BxDF::cos_theta(w_h));
	float denom = 1 - cos_theta * cos_theta;
	pdf_val = 0;
	if (denom > 0 && w_o.dot(w_h) > 0){
		float exponent = (exp_x * w_h.x * w_h.x + exp_y * w_h.y * w_h.y) / denom;
		float d = std::sqrt((exp_x + 1) * (exp_y + 1)) * INV_TAU * std::pow(cos_theta, exponent);
		pdf_val = d / (4 * w_o.dot(w_h));
	}
}
float AnisotropicDistribution::pdf(const Vector &w_o, const Vector &w_i) const {
	Vector w_h = (w_o + w_i).normalized();
	float cos_theta = std::abs(BxDF::cos_theta(w_h));
	float denom = 1 - cos_theta * cos_theta;
	float pdf_val = 0;
	if (denom > 0 && w_o.dot(w_h) > 0){
		float exponent = (exp_x * w_h.x * w_h.x + exp_y * w_h.y * w_h.y) / denom;
		float d = std::sqrt((exp_x + 1) * (exp_y + 1)) * INV_TAU * std::pow(cos_theta, exponent);
		pdf_val = d / (4 * w_o.dot(w_h));
	}
	return pdf_val;
}
void AnisotropicDistribution::sample_first_quadrant(float u_0, float u_1, float &phi, float &cos_theta) const {
	if (exp_x == exp_y){
		phi = PI * u_0 * 0.5;
	}
	else {
		phi = std::atan(std::sqrt((exp_x + 1) / (exp_y + 1)) * std::tan(PI * u_1 * 0.5));
	}
	float cos_phi = std::cos(phi);
	float sin_phi = std::sin(phi);
	cos_theta = std::pow(u_1, 1 / (exp_x * cos_phi * cos_phi + exp_y * sin_phi * sin_phi + 1));
}

