#ifndef POINT_H
#define POINT_H

#include <cmath>
#include <iostream>
#include "vector.h"

struct Point;
inline Point operator+(const Point &p, const Vector &v);
inline Point operator+(const Point &a, const Point &b);
inline Point operator-(const Point &p, const Vector &v);
inline Vector operator-(const Point &a, const Point &b);
inline Point operator*(const Point &p, float s);
inline Point operator*(float s, const Point &p);
inline Point operator/(const Point &p, float s);
inline bool operator==(const Point &a, const Point &b);
inline bool operator!=(const Point &a, const Point &b);
inline std::ostream& operator<<(std::ostream &os, const Point &p);

/*
 * A point in 3D space
 */
struct Point {
	float x, y, z;

	inline Point(float x = 0) : x(x), y(x), z(x) {}
	inline Point(float x, float y, float z) : x(x), y(y), z(z) {}
	inline float distance_sqr(const Point &p){
		return (*this - p).length_sqr();
	}
	inline float distance(const Point &p){
		return (*this - p).length();
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
	inline Point& operator+=(const Vector &v){
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	inline Point& operator-=(const Vector &v){
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	inline Point& operator/=(float s){
		x /= s;
		y /= s;
		z /= s;
		return *this;
	}
	inline Point operator-(){
		return Point{-x, -y, -z};
	}
};
inline Point operator+(const Point &p, const Vector &v){
	return Point{p.x + v.x, p.y + v.y, p.z + v.z};
}
inline Point operator+(const Point &a, const Point &b){
	return Point{a.x + b.x, a.y + b.y, a.z + b.z};
}
inline Point operator-(const Point &p, const Vector &v){
	return Point{p.x - v.x, p.y - v.y, p.z - v.z};
}
inline Vector operator-(const Point &a, const Point &b){
	return Vector{a.x - b.x, a.y - b.y, a.z - b.z};
}
inline Point operator*(const Point &p, float s){
	return Point{p.x * s, p.y * s, p.z * s};
}
inline Point operator*(float s, const Point &p){
	return p * s;
}
inline Point operator/(const Point &p, float s){
	return Point{p.x / s, p.y / s, p.z / s};
}
inline bool operator==(const Point &a, const Point &b){
	return a.x == b.x && a.y == b.y && a.z == b.z;
}
inline bool operator!=(const Point &a, const Point &b){
	return !(a == b);
}
inline std::ostream& operator<<(std::ostream &os, const Point &p){
	os << "p [ " << p.x << ", " << p.y << ", " << p.z << " ]";
	return os;
}

Vector::Vector(const Point &p) : x(p.x), y(p.y), z(p.z){}

#endif

