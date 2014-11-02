#include <cmath>
#include "linalg/util.h"
#include "monte_carlo/util.h"

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

