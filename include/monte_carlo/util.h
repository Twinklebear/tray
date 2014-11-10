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
inline constexpr float uniform_hemisphere_pdf(){
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
inline constexpr float cos_hemisphere_pdf(float cos_theta){
	return cos_theta * INV_PI;
}
/*
 * Uniformly sample a sphere given input random variables in range [0, 1)
 */
Vector uniform_sample_sphere(const std::array<float, 2> &u);
/*
 * Compute the PDF of the sphere sampling
 */
inline constexpr float uniform_sphere_pdf(){
	return 1.f / (4.f * PI);
}
/*
 * Uniformly sample the barycentric coordinates of a triangle, this assumes an isoscles right-triangle
 * sample positions should be [0, 1)
 */
Vector uniform_sample_tri(const std::array<float, 2> &u);
/*
 * Compute concentric sample positions on a unit disk mapping input from range [0, 1)
 * to sample positions on a disk
 * See: https://mediatech.aalto.fi/~jaakko/T111-5310/K2013/JGT-97.pdf
 */
std::array<float, 2> concentric_sample_disk(const std::array<float, 2> &u);
/*
 * Uniformly sample a vector on a cone with max angle cos_theta where the
 * cone is centered about the z axis
 */
Vector uniform_sample_cone(const std::array<float, 2> &u, float cos_theta);
/*
 * Uniformly sample a vector on a cone with max angle cos_theta where
 * the cone is centered about the z vector in the coordinate system passed
 */
Vector uniform_sample_cone(const std::array<float, 2> &u, float cos_theta, const Vector &x,
	const Vector &y, const Vector &z);
/*
 * Compute the PDF for uniformly sampling a cone with some max angle
 */
inline constexpr float uniform_cone_pdf(float cos_theta){
	return 1 / (TAU * (1 - cos_theta));
}
/*
 * Balance heuristic for multiple importance sampling for two functions being sampled, f & g
 * n_f, n_g - number of samples taken of each
 * pdf_f, pdf_g - pdf of each function
 */
inline float balance_heuristic(float n_f, float pdf_f, float n_g, float pdf_g){
	return (n_f * pdf_f) / (n_f * pdf_f + n_g * pdf_g);
}
/*
 * Power heuristic for multiple importance sampling for two functions being sampled, f & g
 * where beta is hard-coded to be two following PBR & Veach
 * n_f, n_g - number of samples taken of each
 * pdf_f, pdf_g - pdf of each function
 */
inline float power_heuristic(float n_f, float pdf_f, float n_g, float pdf_g){
	float f = n_f * pdf_f;
	float g = n_g * pdf_g;
	return (f * f) / (f * f + g * g);
}

#endif

