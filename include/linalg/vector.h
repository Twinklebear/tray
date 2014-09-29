#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>
#include <iostream>

struct Vector;
struct Normal;
struct Point;
inline Vector operator+(const Vector &a, const Vector &b);
inline Vector operator-(const Vector &a, const Vector &b);
inline Vector operator*(const Vector &v, float s);
inline Vector operator*(float s, const Vector &v);
inline Vector operator/(const Vector &v, float s);
inline Vector operator/(const Vector &a, const Vector &b);
inline Vector operator-(const Vector &v);
inline bool operator==(const Vector &a, const Vector &b);
inline bool operator!=(const Vector &a, const Vector &b);
inline std::ostream& operator<<(std::ostream &os, const Vector &v);

/*
 * A vector in 3D space
 */
struct Vector {
	float x, y, z;

	inline Vector(float x = 0) : x(x), y(x), z(x) {}
	inline Vector(float x, float y, float z) : x(x), y(y), z(z) {}
	inline explicit Vector(const Normal &n);
	inline explicit Vector(const Point &p);
	inline float dot(const Vector &v) const {
		return x * v.x + y * v.y + z * v.z;
	}
	inline float dot(const Normal &n) const;
	inline Vector cross(const Vector &v) const {
		return Vector{y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x};
	}
	inline float length_sqr() const {
		return x * x + y * y + z * z;
	}
	inline float length() const {
		return std::sqrt(length_sqr());
	}
	inline Vector normalized() const {
		return *this / length();
	}
	inline const float& operator[](size_t i) const {
		switch (i){
			case 0: return x;
			case 1: return y;
			default: return z;
		}
	}
	inline float& operator[](size_t i){
		switch (i){
			case 0: return x;
			case 1: return y;
			default: return z;
		}
	}
	inline Vector& operator+=(const Vector &v){
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	inline Vector& operator-=(const Vector &v){
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	inline Vector& operator*=(float s){
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}
	inline Vector& operator/=(float s){
		return *this *= 1.f / s;
	}
	inline Vector operator-(){
		return Vector{-x, -y, -z};
	}
};
inline Vector operator+(const Vector &a, const Vector &b){
	return Vector{a.x + b.x, a.y + b.y,  a.z + b.z};
}
inline Vector operator-(const Vector &a, const Vector &b){
	return Vector{a.x - b.x, a.y - b.y,  a.z - b.z};
}
inline Vector operator*(const Vector &v, float s){
	return Vector{v.x * s, v.y * s, v.z * s};
}
inline Vector operator*(float s, const Vector &v){
	return v * s;
}
inline Vector operator/(const Vector &v, float s){
	return v * (1 / s);
}
inline Vector operator/(const Vector &a, const Vector &b){
	return Vector{a.x / b.x, a.y / b.y, a.z / b.z};
}
inline Vector operator-(const Vector &v){
	return Vector{-v.x, -v.y, -v.z};
}
inline bool operator==(const Vector &a, const Vector &b){
	return a.x == b.x && a.y == b.y && a.z == b.z;
}
inline bool operator!=(const Vector &a, const Vector &b){
	return !(a == b);
}
inline std::ostream& operator<<(std::ostream &os, const Vector &v){
	os << "v [ " << v.x << ", " << v.y << ", " << v.z << " ]";
	return os;
}

inline Normal operator+(const Normal &a, const Normal &b);
inline Normal operator-(const Normal &a, const Normal &b);
inline Normal operator*(const Normal &n, float s);
inline Normal operator*(float s, const Normal &n);
inline Normal operator/(const Normal &n, float s);
inline Normal operator-(const Normal &n);
inline bool operator==(const Normal &a, const Normal &b);
inline bool operator!=(const Normal &a, const Normal &b);
inline std::ostream& operator<<(std::ostream &os, const Normal &n);

/*
 * A surface normal in 3D space
 * Although this implementation is almost identical to Vector
 * normals behave differently under transformations and this
 * distinction is important
 */
struct Normal {
	float x, y, z;

	inline Normal(float x = 0) : x(x), y(x), z(x) {}
	inline Normal(float x, float y, float z) : x(x), y(y), z(z) {}
	inline explicit Normal(const Vector &v) : x(v.x), y(v.y), z(v.z) {}
	inline explicit Normal(const Point &p);
	inline float dot(const Vector &v) const {
		return x * v.x + y * v.y + z * v.z;
	}
	inline float dot(const Normal &n) const {
		return x * n.x + y * n.y + z * n.z;
	}
	inline float length_sqr() const {
		return x * x + y * y + z * z;
	}
	inline float length() const {
		return std::sqrt(length_sqr());
	}
	inline Normal normalized() const {
		return *this / length();
	}
	inline Normal face_forward(const Vector &v) const {
		return dot(v) < 0.f ? -(*this) : *this;
	}
	inline const float& operator[](size_t i) const {
		switch (i){
			case 0: return x;
			case 1: return y;
			default: return z;
		}
	}
	inline float& operator[](size_t i){
		switch (i){
			case 0: return x;
			case 1: return y;
			default: return z;
		}
	}
	inline Normal& operator+=(const Normal &n){
		x += n.x;
		y += n.y;
		z += n.z;
		return *this;
	}
	inline Normal& operator-=(const Normal &n){
		x -= n.x;
		y -= n.y;
		z -= n.z;
		return *this;
	}
	inline Normal& operator*=(float s){
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}
	inline Normal& operator/=(float s){
		return *this *= 1 / s;
	}
	inline Normal operator-(){
		return Normal{-x, -y, -z};
	}
};
inline Normal operator+(const Normal &a, const Normal &b){
	return Normal{a.x + b.x, a.y + b.y,  a.z + b.z};
}
inline Normal operator-(const Normal &a, const Normal &b){
	return Normal{a.x - b.x, a.y - b.y,  a.z - b.z};
}
inline Normal operator*(const Normal &n, float s){
	return Normal{n.x * s, n.y * s, n.z * s};
}
inline Normal operator*(float s, const Normal &n){
	return n * s;
}
inline Normal operator/(const Normal &n, float s){
	return n * (1 / s);
}
inline Normal operator-(const Normal &n){
	return Normal{-n.x, -n.y, -n.z};
}
inline bool operator==(const Normal &a, const Normal &b){
	return a.x == b.x && a.y == b.y && a.z == b.z;
}
inline bool operator!=(const Normal &a, const Normal &b){
	return !(a == b);
}
inline std::ostream& operator<<(std::ostream &os, const Normal &n){
	os << "n [ " << n.x << ", " << n.y << ", " << n.z << " ]";
	return os;
}

inline Vector::Vector(const Normal &n) : x(n.x), y(n.y), z(n.z) {}
inline float Vector::dot(const Normal &n) const {
	return x * n.x + y * n.y + z * n.z;
}

#endif

