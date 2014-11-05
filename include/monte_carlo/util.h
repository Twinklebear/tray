#ifndef MONTE_CARLO_UTIL_H
#define MONTE_CARLO_UTIL_H

#include <array>
#include "linalg/util.h"
#include "linalg/vector.h"

/*
 * Uniformly sample a hemisphere given input random variables in the range [0, 1)
 */
Vector uniform_sample_hemisphere(const std::array<float, 2> &u);
/*
 * Compute the PDF of the uniform hemisphere sampling
 */
constexpr float uniform_hemisphere_pdf(){
	return INV_TAU;
}
/*
 * Sample a hemisphere using a cosine distribution to produce cosine weighted samples
 * input samples should be in range [0, 1)
 * directions returned will be in the hemisphere around (0, 0, 1)
 */
Vector cos_sample_hemisphere(const std::array<float, 2> &u);
/*
 * Compute the PDF of the cosine weighted hemisphere sampling
 */
constexpr float cos_hemisphere_pdf(float cos_theta){
	return cos_theta * INV_PI;
}
/*
 * Uniformly sample a sphere given input random variables in range [0, 1)
 */
Vector uniform_sample_sphere(const std::array<float, 2> &u);
/*
 * Compute the PDF of the sphere sampling
 */
constexpr float uniform_sphere_pdf(){
	return 1.f / (4.f * PI);
}
/*
 * Uniformly sample positions on a triangle, this assumes an isoscles right-triangle
 * sample positions should be [0, 1)
 */
std::array<float, 2> uniform_sample_tri(const std::array<float, 2> &u);
/*
 * Compute concentric sample positions on a unit disk mapping input from range [0, 1)
 * to sample positions on a disk
 * See: https://mediatech.aalto.fi/~jaakko/T111-5310/K2013/JGT-97.pdf
 */
std::array<float, 2> concentric_sample_disk(const std::array<float, 2> &u);

#endif

