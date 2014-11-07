#include <cmath>
#include "monte_carlo/util.h"


Vector uniform_sample_hemisphere(const std::array<float, 2> &u){
	float r = std::sqrt(std::max(0.f, 1.f - u[0] * u[0]));
	float phi = TAU * u[1];
	return Vector{std::cos(phi) * r, std::sin(phi) * r, u[0]};
}
Vector cos_sample_hemisphere(const std::array<float, 2> &u){
	//We use Malley's method here, generate samples on a disk then project
	//them up to the hemisphere
	std::array<float, 2> d = concentric_sample_disk(u);
	return Vector{d[0], d[1], std::sqrt(std::max(0.f, 1.f - d[0] * d[0] - d[1] * d[1]))};
}
Vector uniform_sample_sphere(const std::array<float, 2> &u){
	float z = 1 - 2 * u[0];
	float r = std::sqrt(std::max(0.f, 1.f - z * z));
	float phi = TAU * u[1];
	return Vector{std::cos(phi) * r, std::sin(phi) * r, z};
}
std::array<float, 2> uniform_sample_tri(const std::array<float, 2> &u){
	float su = std::sqrt(u[0]);
	return {1 - su, u[1] * su};
}
std::array<float, 2> concentric_sample_disk(const std::array<float, 2> &u){
	std::array<float, 2> s{2 * u[0] - 1, 2 * u[1] - 1};
	float radius, theta;
	if (s[0] == 0 && s[1] == 0){
		return {0.f, 0.f};
	}
	if (s[0] >= -s[1]){
		if (s[0] > s[1]){
			radius = s[0];
			theta = s[1] > 0 ? s[1] / s[0] : 8 + s[1] / s[0];
		}
		else {
			radius = s[1];
			//Is this test correct here? Paper doesn't use it
			theta = 2 - s[0] / s[1];
		}
	}
	else {
		if (s[0] <= s[1]){
			radius = -s[0];
			theta = 4 + s[1] / s[0];
		}
		else {
			radius = -s[1];
			if (s[1] != 0){
				theta = 6 - s[0] / s[1];
			}
			else {
				theta = 0;
			}
		}
	}
	theta *= PI / 4.f;
	return {radius * std::cos(theta), radius * std::sin(theta)};
}
Vector uniform_sample_cone(const std::array<float, 2> &u, float cos_theta){
	cos_theta = lerp(u[0], cos_theta, 1);
	float sin_theta = std::sqrt(1 - cos_theta * cos_theta);
	float phi = u[1] * TAU;
	return Vector{std::cos(phi) * sin_theta, std::sin(phi) * sin_theta, cos_theta};
}
Vector uniform_sample_cone(const std::array<float, 2> &u, float cos_theta, const Vector &x,
	const Vector &y, const Vector &z)
{
	cos_theta = lerp(u[0], cos_theta, 1);
	float sin_theta = std::sqrt(1 - cos_theta * cos_theta);
	float phi = u[1] * TAU;
	return std::cos(phi) * sin_theta * x + std::sin(phi) * sin_theta * y + cos_theta * z;
}

