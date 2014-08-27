#ifndef GEOM_UTIL_H
#define GEOM_UTIL_H

#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/*
 * Some basic math/geometric utility functions
 */
inline float lerp(float t, float a, float b){
	return (1.f - t) * a + t * b;
}
inline float clamp(float x, float l, float h){
	return x < l ? l : x > h ? h : x;
}
//Version of mod that handles negatives cleaner, % is undefined in this case
inline int mod(int a, int m){
	int n = int{a / m};
	a -= n * m;
	return a < 0 ? a + m : a;
}
inline float radians(float deg){
	return M_PI / 180.f * deg;
}
inline float degrees(float rad){
	return 180.f / M_PI * rad;
}
inline float log_2(float x){
	static float inv_log2 = 1.f / std::log(2);
	return std::log(x) * inv_log2;
}
/*
 * Solve a quadratic equation a*t^2 + b*2 + c = 0 and return real roots
 * in t0, t1. Returns false if no real roots exist
 */
inline bool solve_quadratic(float a, float b, float c, float &t0, float &t1){
	float discrim = b * b - 4 * a * c;
	if (discrim < 0){
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

#endif

