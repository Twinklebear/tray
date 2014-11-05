#include <cmath>
#include "monte_carlo/util.h"


Vector uniform_sample_hemisphere(const std::array<float, 2> &u){
	float r = std::sqrt(std::max(0.f, 1.f - u[0] * u[0]));
	float phi = TAU * u[1];
	return Vector{std::cos(phi) * r, std::sin(phi) * r, u[0]};
}
Vector uniform_sample_sphere(const std::array<float, 2> &u){
	float z = 1 - 2 * u[0];
	float r = std::sqrt(std::max(0.f, 1.f - z * z));
	float phi = TAU * u[1];
	return Vector{std::cos(phi) * r, std::sin(phi) * r, z};
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

