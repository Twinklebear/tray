#ifndef QUATERNION_H
#define QUATERNION_H

#include <cmath>
#include "vector.h"
#include "transform.h"

struct Quaternion;
inline Quaternion operator+(const Quaternion &a, const Quaternion &b);
inline Quaternion operator-(const Quaternion &a, const Quaternion &b);
inline Quaternion operator*(const Quaternion &a, const Quaternion &b);
inline Quaternion operator*(const Quaternion &q, float f);
inline Quaternion operator*(float f, const Quaternion &q);
inline Quaternion operator/(const Quaternion &q, float f);

/*
 * A Quaternion that can be operated on and converted into a transform
 */
struct Quaternion {
	Vector v;
	float w;

	Quaternion(const Vector &v = Vector{0, 0, 0}, float w = 0) : v(v), w(w)
	{}
	Quaternion(const Transform &t);
	/*
	 * Compute the dot product of two quaternions
	 */
	inline float dot(const Quaternion &b){
		return v.dot(b.v) + w * b.w;
	}
	inline Quaternion normalized(){
		return *this / std::sqrt(dot(*this));
	}
	Transform to_transform() const;
	inline Quaternion& operator+=(const Quaternion &q){
		v += q.v;
		w += q.w;
		return *this;
	}
	inline Quaternion& operator-=(const Quaternion &q){
		v -= q.v;
		w -= q.w;
		return *this;
	}
	inline Quaternion& operator*=(const Quaternion &q){
		v = v.cross(q.v) + w * q.v + q.w * v;
		w = w * q.w - v.dot(q.v);
		return *this;
	}
	inline Quaternion& operator*=(float f){
		v *= f;
		w *= f;
		return *this;
	}
	inline Quaternion& operator/=(float f){
		v /= f;
		w /= f;
		return *this;
	}
};

inline Quaternion operator+(const Quaternion &a, const Quaternion &b){
	return Quaternion{a.v + b.v, a.w + b.w};
}
inline Quaternion operator-(const Quaternion &a, const Quaternion &b){
	return Quaternion{a.v - b.v, a.w - b.w};
}
inline Quaternion operator*(const Quaternion &a, const Quaternion &b){
	return Quaternion{a.v.cross(b.v) + a.w * b.v + b.w * a.v,
		a.w * b.w - a.v.dot(b.v)};
}
inline Quaternion operator*(const Quaternion &q, float f){
	return Quaternion{q.v * f, q.w * f};
}
inline Quaternion operator*(float f, const Quaternion &q){
	return Quaternion{q.v * f, q.w * f};
}
inline Quaternion operator/(const Quaternion &q, float f){
	return Quaternion{q.v / f, q.w / f};
}

#endif

