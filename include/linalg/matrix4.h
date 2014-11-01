#ifndef MATRIX4_H
#define MATRIX4_H

#include <ostream>
#include <array>

class Matrix4 {
	std::array<float, 16> mat;

public:
	//Initializes the matrix to the identity
	Matrix4();
	//Create from some existing matrix, m should be row-major
	Matrix4(const std::array<float, 16> &m);
	Matrix4 inverse() const;
	Matrix4 transpose() const;
	//Get the value of the matrix at row i, col j
	inline const float& at(size_t i, size_t j) const {
		return mat[4 * i + j];
	}
	inline float& at(size_t i, size_t j){
		return mat[4 * i + j];
	}
	//Get a pointer to row i of the matrix
	inline const float* operator[](size_t i) const {
		return &mat[4 * i];
	}
	inline float* operator[](size_t i){
		return &mat[4 * i];
	}
	bool operator==(const Matrix4 &m) const;
	bool operator!=(const Matrix4 &m) const;
	Matrix4 operator+(const Matrix4 &m) const;
	Matrix4 operator-(const Matrix4 &m) const;
	Matrix4 operator*(const Matrix4 &m) const;
	Matrix4 operator*(float s) const;
	Matrix4& operator+=(const Matrix4 &m);
	Matrix4& operator-=(const Matrix4 &m);
	Matrix4& operator*=(const Matrix4 &m);
	void print(std::ostream &os) const;
};

Matrix4 operator*(float s, const Matrix4 &m);
std::ostream& operator<<(std::ostream &os, const Matrix4 &m);

#endif

