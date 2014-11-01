#include <iostream>
#include <array>
#include <cassert>
#include "linalg/matrix4.h"

Matrix4::Matrix4(){
	mat = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
}
Matrix4::Matrix4(const std::array<float, 16> &m) : mat(m) {}
Matrix4 Matrix4::inverse() const {
	//MESA's matrix inverse, tweaked for row-major matrices
	Matrix4 inv;
	float det;
	inv.mat[0] = mat[5] * mat[10] * mat[15]
		- mat[5]  * mat[11] * mat[14]
		- mat[9]  * mat[6]  * mat[15]
		+ mat[9]  * mat[7]  * mat[14]
		+ mat[13] * mat[6]  * mat[11]
		- mat[13] * mat[7]  * mat[10];

	inv.mat[4] = -mat[4]  * mat[10] * mat[15]
		+ mat[4]  * mat[11] * mat[14]
		+ mat[8]  * mat[6]  * mat[15]
		- mat[8]  * mat[7]  * mat[14]
		- mat[12] * mat[6]  * mat[11]
		+ mat[12] * mat[7]  * mat[10];

	inv.mat[8] = mat[4]  * mat[9] * mat[15]
		- mat[4]  * mat[11] * mat[13]
		- mat[8]  * mat[5] * mat[15]
		+ mat[8]  * mat[7] * mat[13]
		+ mat[12] * mat[5] * mat[11]
		- mat[12] * mat[7] * mat[9];

	inv.mat[12] = -mat[4]  * mat[9] * mat[14]
		+ mat[4]  * mat[10] * mat[13]
		+ mat[8]  * mat[5] * mat[14]
		- mat[8]  * mat[6] * mat[13]
		- mat[12] * mat[5] * mat[10]
		+ mat[12] * mat[6] * mat[9];

	inv.mat[1] = -mat[1]  * mat[10] * mat[15]
		+ mat[1]  * mat[11] * mat[14]
		+ mat[9]  * mat[2] * mat[15]
		- mat[9]  * mat[3] * mat[14]
		- mat[13] * mat[2] * mat[11]
		+ mat[13] * mat[3] * mat[10];

	inv.mat[5] = mat[0]  * mat[10] * mat[15]
		- mat[0]  * mat[11] * mat[14]
		- mat[8]  * mat[2] * mat[15]
		+ mat[8]  * mat[3] * mat[14]
		+ mat[12] * mat[2] * mat[11]
		- mat[12] * mat[3] * mat[10];

	inv.mat[9] = -mat[0]  * mat[9] * mat[15]
		+ mat[0]  * mat[11] * mat[13]
		+ mat[8]  * mat[1] * mat[15]
		- mat[8]  * mat[3] * mat[13]
		- mat[12] * mat[1] * mat[11]
		+ mat[12] * mat[3] * mat[9];

	inv.mat[13] = mat[0]  * mat[9] * mat[14]
		- mat[0]  * mat[10] * mat[13]
		- mat[8]  * mat[1] * mat[14]
		+ mat[8]  * mat[2] * mat[13]
		+ mat[12] * mat[1] * mat[10]
		- mat[12] * mat[2] * mat[9];

	inv.mat[2] = mat[1]  * mat[6] * mat[15]
		- mat[1]  * mat[7] * mat[14]
		- mat[5]  * mat[2] * mat[15]
		+ mat[5]  * mat[3] * mat[14]
		+ mat[13] * mat[2] * mat[7]
		- mat[13] * mat[3] * mat[6];

	inv.mat[6] = -mat[0]  * mat[6] * mat[15]
		+ mat[0]  * mat[7] * mat[14]
		+ mat[4]  * mat[2] * mat[15]
		- mat[4]  * mat[3] * mat[14]
		- mat[12] * mat[2] * mat[7]
		+ mat[12] * mat[3] * mat[6];

	inv.mat[10] = mat[0]  * mat[5] * mat[15]
		- mat[0]  * mat[7] * mat[13]
		- mat[4]  * mat[1] * mat[15]
		+ mat[4]  * mat[3] * mat[13]
		+ mat[12] * mat[1] * mat[7]
		- mat[12] * mat[3] * mat[5];

	inv.mat[14] = -mat[0]  * mat[5] * mat[14]
		+ mat[0]  * mat[6] * mat[13]
		+ mat[4]  * mat[1] * mat[14]
		- mat[4]  * mat[2] * mat[13]
		- mat[12] * mat[1] * mat[6]
		+ mat[12] * mat[2] * mat[5];

	inv.mat[3] = -mat[1] * mat[6] * mat[11]
		+ mat[1] * mat[7] * mat[10]
		+ mat[5] * mat[2] * mat[11]
		- mat[5] * mat[3] * mat[10]
		- mat[9] * mat[2] * mat[7]
		+ mat[9] * mat[3] * mat[6];

	inv.mat[7] = mat[0] * mat[6] * mat[11]
		- mat[0] * mat[7] * mat[10]
		- mat[4] * mat[2] * mat[11]
		+ mat[4] * mat[3] * mat[10]
		+ mat[8] * mat[2] * mat[7]
		- mat[8] * mat[3] * mat[6];

	inv.mat[11] = -mat[0] * mat[5] * mat[11]
		+ mat[0] * mat[7] * mat[9]
		+ mat[4] * mat[1] * mat[11]
		- mat[4] * mat[3] * mat[9]
		- mat[8] * mat[1] * mat[7]
		+ mat[8] * mat[3] * mat[5];

	inv.mat[15] = mat[0] * mat[5] * mat[10]
		- mat[0] * mat[6] * mat[9]
		- mat[4] * mat[1] * mat[10]
		+ mat[4] * mat[2] * mat[9]
		+ mat[8] * mat[1] * mat[6]
		- mat[8] * mat[2] * mat[5];

	det = mat[0] * inv.mat[0] + mat[1] * inv.mat[4]
		+ mat[2] * inv.mat[8] + mat[3] * inv.mat[12];
	assert(det != 0);
	det = 1.0 / det;

	for (size_t i = 0; i < 16; ++i){
		inv.mat[i] *= det;
	}
	return inv;

}
Matrix4 Matrix4::transpose() const {
	Matrix4 t;
	for (size_t i = 0; i < 4; ++i){
		for (size_t j = 0; j < 4; ++j){
			t.at(i, j) = at(j, i);
		}
	}
	return t;
}
bool Matrix4::operator==(const Matrix4 &m) const {
	return mat == m.mat;
}
bool Matrix4::operator!=(const Matrix4 &m) const {
	return mat != m.mat;
}
Matrix4 Matrix4::operator+(const Matrix4 &m) const {
	Matrix4 res;
	for (size_t i = 0; i < 16; ++i){
		res.mat[i] = mat[i] + m.mat[i];
	}
	return res;
}
Matrix4 Matrix4::operator-(const Matrix4 &m) const {
	Matrix4 res;
	for (size_t i = 0; i < 16; ++i){
		res.mat[i] = mat[i] - m.mat[i];
	}
	return res;
}
Matrix4 Matrix4::operator*(const Matrix4 &m) const {
	Matrix4 res;
	for (size_t i = 0; i < 4; ++i){
		for (size_t j = 0; j < 4; ++j){
			res.at(i, j) = at(i, 0) * m.at(0, j)
				+ at(i, 1) * m.at(1, j)
				+ at(i, 2) * m.at(2, j)
				+ at(i, 3) * m.at(3, j);
		}
	}
	return res;
}
Matrix4 Matrix4::operator*(float s) const {
	Matrix4 m{mat};
	for (auto &f : m.mat){
		f *= s;
	}
	return m;
}
Matrix4& Matrix4::operator+=(const Matrix4 &m){
	for (size_t i = 0; i < 16; ++i){
		mat[i] += m.mat[i];
	}
	return *this;
}
Matrix4& Matrix4::operator-=(const Matrix4 &m){
	for (size_t i = 0; i < 16; ++i){
		mat[i] -= m.mat[i];
	}
	return *this;
}
Matrix4& Matrix4::operator*=(const Matrix4 &m){
	this->mat = (*this * m).mat;
	return *this;
}
void Matrix4::print(std::ostream &os) const {
	os << "Matrix4 { ";
	for (size_t i = 0; i < 4; ++i){
		os << "[ ";
		for (size_t j = 0; j < 4; ++j){
			os << at(i, j);
			if (j < 3){
				os << ", ";
			}
		}
		os << "], ";
	}
	os << "}";
}
Matrix4 operator*(float s, const Matrix4 &m){
	return m * s;
}
std::ostream& operator<<(std::ostream &os, const Matrix4 &m){
	m.print(os);
	return os;
}

