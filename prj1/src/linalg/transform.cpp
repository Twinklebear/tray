#include <cmath>
#include "linalg/util.h"
#include "linalg/vector.h"
#include "linalg/point.h"
#include "linalg/ray.h"
#include "linalg/matrix4.h"
#include "linalg/transform.h"

Transform::Transform(){}
Transform::Transform(const Matrix4 &mat) : mat(mat), inv(mat.inverse()) {}
Transform::Transform(const Matrix4 &mat, const Matrix4 &inv) : mat(mat), inv(inv) {}
Transform Transform::translate(const Vector &v){
	return Transform{
		Matrix4{{
			1, 0, 0, v.x,
			0, 1, 0, v.y,
			0, 0, 1, v.z,
			0, 0, 0, 1
		}},
		Matrix4{{
			1, 0, 0, -v.x,
			0, 1, 0, -v.y,
			0, 0, 1, -v.z,
			0, 0, 0, 1
		}}
	};
}
Transform Transform::scale(float x, float y, float z){
	return Transform{
		Matrix4{{
			x, 0, 0, 0,
			0, y, 0, 0,
			0, 0, z, 0,
			0, 0, 0, 1
		}},
		Matrix4{{
			1/x, 0, 0, 0,
			0, 1/y, 0, 0,
			0, 0, 1/z, 0,
			0, 0, 0, 1
		}}
	};
}
Transform Transform::rotate_x(float deg){
	float s = std::sin(radians(deg));
	float c = std::cos(radians(deg));
	Matrix4 m{{
		1, 0, 0, 0,
		0, c, -s, 0,
		0, s, c, 0,
		0, 0, 0, 1
	}};
	return Transform(m, m.transpose());
}
Transform Transform::rotate_y(float deg){
	float s = std::sin(radians(deg));
	float c = std::cos(radians(deg));
	Matrix4 m{{
		c, 0, s, 0,
		0, 1, 0, 0,
		-s, 0, c, 0,
		0, 0, 0, 1
	}};
	return Transform(m, m.transpose());
}
Transform Transform::rotate_z(float deg){
	float s = std::sin(radians(deg));
	float c = std::cos(radians(deg));
	Matrix4 m{{
		c, -s, 0, 0,
		s, c, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	}};
	return Transform(m, m.transpose());
}
Transform Transform::rotate(const Vector &axis, float deg){
	Vector a = axis.normalized();
	float s = std::sin(radians(deg));
	float c = std::cos(radians(deg));
	Matrix4 m;
	m[0][0] = a.x * a.x + (1 - a.x * a.x) * c;
	m[0][1] = a.x * a.y * (1 - c) - a.z * s;
	m[0][2] = a.x * a.z * (1 - c) + a.y * s;
	m[0][3] = 0;

	m[1][0] = a.x * a.y * (1 - c) + a.z * s;
	m[1][1] = a.y * a.y + (1 - a.y * a.y) * c;
	m[1][2] = a.y * a.z * (1 - c) - a.x * s;
	m[1][3] = 0;

	m[2][0] = a.x * a.z * (1 - c) - a.y * s;
	m[2][1] = a.y * a.z * (1 - c) + a.x * s;
	m[2][2] = a.z * a.z + (1 - a.z * a.z) * c;
	m[2][3] = 0;

	m[3][0] = 0;
	m[3][1] = 0;
	m[3][2] = 0;
	m[3][3] = 1;
	return Transform{m, m.transpose()};
}
Transform Transform::look_at(const Point &pos, const Point &center, const Vector &up){
	Matrix4 m;
	Vector dir = (center - pos).normalized();
	Vector left = up.normalized().cross(dir).normalized();
	//The up vector may not be quite correct so find the exact one
	Vector new_up = dir.cross(left);
	for (int i = 0; i < 3; ++i){
		m[i][0] = left[i];
		m[i][1] = new_up[i];
		m[i][2] = dir[i];
		m[i][3] = pos[i];
	}
	m[0][3] = 0;
	m[1][3] = 0;
	m[2][3] = 0;
	m[3][3] = 1;
	return Transform(m.inverse(), m);
}
Transform Transform::perspective(float fov, float near, float far){
	Matrix4 proj_div{{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, far / (far - near), -far * near / (far - near),
		0, 0, 1, 0
	}};
	float inv_tan = 1 / std::tan(radians(fov) / 2);
	return scale(inv_tan, inv_tan, 1) * Transform{proj_div};
}
Transform Transform::orthographic(float near, float far){
	return scale(1, 1, 1 / (far - near)) * translate(Vector{0, 0, -near});
}
Transform Transform::inverse() const {
	return Transform{inv, mat};
}
bool Transform::operator==(const Transform &t) const {
	return mat == t.mat && inv == t.inv;
}
bool Transform::operator!=(const Transform &t) const {
	return mat != t.mat || inv != t.inv;
}
Point Transform::operator()(const Point &p) const {
	Point b;
	(*this)(p, b);
	return b;
}
void Transform::operator()(const Point &in, Point &out) const {
	float x = in.x;
	float y = in.y;
	float z = in.z;
	for (int i = 0; i < 3; ++i){
		out[i] = mat[i][0] * x + mat[i][1] * y + mat[i][2] * z + mat[i][3];
	}
	float w = mat[3][0] * x + mat[3][1] * y + mat[3][2] * z + mat[3][3];
	if (w != 1.f){
		out /= w;
	}
}
Vector Transform::operator()(const Vector &v) const {
	Vector b;
	(*this)(v, b);
	return b;
}
void Transform::operator()(const Vector &in, Vector &out) const {
	float x = in.x;
	float y = in.y;
	float z = in.z;
	for (int i = 0; i < 3; ++i){
		out[i] = mat[i][0] * x + mat[i][1] * y + mat[i][2] * z;
	}
}
Normal Transform::operator()(const Normal &n) const {
	Normal b;
	(*this)(n, b);
	return b;
}
void Transform::operator()(const Normal &in, Normal &out) const {
	float x = in.x;
	float y = in.y;
	float z = in.z;
	for (int i = 0; i < 3; ++i){
		out[i] = inv[0][i] * x + inv[1][i] * y + inv[2][i] * z;
	}
}
Ray Transform::operator()(const Ray &r) const {
	Ray b;
	(*this)(r, b);
	return b;
}
void Transform::operator()(const Ray &in, Ray &out) const {
	(*this)(in.o, out.o);
	(*this)(in.d, out.d);
}
Matrix4 Transform::operator()(const Matrix4 &m) const {
	Matrix4 a;
	(*this)(m, a);
	return a;
}
void Transform::operator()(const Matrix4 &in, Matrix4 &out) const {
	out = mat * in;
}
Transform Transform::operator*(const Transform &t) const {
	return Transform{mat * t.mat, inv * t.inv};
}
Transform& Transform::operator*=(const Transform &t){
	mat *= t.mat;
	inv *= t.inv;
	return *this;
}
void Transform::print(std::ostream &os) const {
	os << "Transform [mat = " << mat
		<< "\n\tinv = " << inv << "]";
}
std::ostream& operator<<(std::ostream &os, const Transform &t){
	t.print(os);
	return os;
}

