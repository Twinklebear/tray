#ifndef LINALG_UTIL_H
#define LINALG_UTIL_H

#include <cmath>
#include <array>
#include <algorithm>
#include "vector.h"

const float PI = 3.14159;
const float TAU = 6.28318;
const float INV_PI = 1.f / PI;
const float INV_TAU = 1.f / TAU;

/*
 * Enum for storing x/y/z axis ids
 */
enum AXIS { X, Y, Z };

/*
 * Some basic math/geometric utility functions
 */
template<typename T>
constexpr T lerp(float t, const T &a, const T &b){
	return (1.f - t) * a + t * b;
}
template<typename T>
constexpr inline T clamp(T x, T l, T h){
	return x < l ? l : x > h ? h : x;
}
//Version of mod that handles negatives cleaner, % is undefined in this case
inline int mod(int a, int m){
	int n = int{a / m};
	a -= n * m;
	return a < 0 ? a + m : a;
}
constexpr inline float radians(float deg){
	return PI / 180.f * deg;
}
constexpr inline float degrees(float rad){
	return 180.f / PI * rad;
}
inline float log_2(float x){
	static float inv_log2 = 1.f / std::log(2);
	return std::log(x) * inv_log2;
}
template<typename T>
constexpr int sign(T x){
	return (T{0} < x) - (x < T{0});
}
inline Vector spherical_dir(float sin_theta, float cos_theta, float phi){
	return Vector{sin_theta * std::cos(phi), sin_theta * std::sin(phi), cos_theta};
}
constexpr inline float spherical_theta(const Vector &v){
	return std::acos(clamp(v.z, -1.f, 1.f));
}
inline float spherical_phi(const Vector &v){
	float p = std::atan2(v.y, v.x);
	return p < 0.f ? p + TAU : p;
}
/*
 * Round x up to the nearest power of 2
 * Based off Stephan Brumme's method
 * http://bits.stephan-brumme.com/roundUpToNextPowerOfTwo.html
 */
inline uint32_t round_up_pow2(uint32_t x){
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x + 1;
}
/*
 * Solve a quadratic equation a*t^2 + b*2 + c = 0 and return real roots
 * in t0, t1. Returns false if no real roots exist
 */
inline bool solve_quadratic(float a, float b, float c, float &t0, float &t1){
	float discrim = b * b - 4 * a * c;
	if (discrim <= 0){
		return false;
	}
	discrim = std::sqrt(discrim);
	float q = b < 0 ? -0.5f * (b - discrim) : -0.5f * (b + discrim);
	t0 = q / a;
	t1 = c / q;
	if (t0 > t1){
		std::swap(t0, t1);
	}
	return true;
}
/*
 * Solve a 2x2 linear system of equations, mat is the 2x2 matrix in row-major order
 * rhs is the right side vector to solve for. Solutions are returned through a and b
 * returns true if the system was solved successfully
 */
inline bool solve_linear2x2(const std::array<float, 4> &mat, const std::array<float, 2> &rhs, float &a, float &b){
	float det = mat[0] * mat[3] - mat[1] * mat[2];
	if (std::abs(det) < 1e-10){
		return false;
	}
	a = (mat[3] * rhs[0] - mat[1] * rhs[1]) / det;
	b = (mat[0] * rhs[1] - mat[2] * rhs[0]) / det;
	if (std::isnan(a) || std::isnan(b)){
		return false;
	}
	return true;
}
/*
 * Compute a local coordinate system from a single vector
 */
inline void coordinate_system(const Vector &e1, Vector &e2, Vector &e3){
	if (std::abs(e1.x) > std::abs(e1.y)){
		float inv_len = 1 / std::sqrt(e1.x * e1.x + e1.z * e1.z);
		e2 = Vector{-e1.z * inv_len, 0, e1.x * inv_len};
	}
	else {
		float inv_len = 1 / std::sqrt(e1.y * e1.y + e1.z * e1.z);
		e2 = Vector{0, e1.z * inv_len, -e1.y * inv_len};
	}
	e3 = e1.cross(e2);
}

#endif

