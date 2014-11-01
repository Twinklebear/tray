#include <cmath>
#include <array>
#include "linalg/quaternion.h"

Quaternion::Quaternion(const Transform &t){
	//See Ken Shoemake - Quaternions (1991)
	float trace = t.mat[0][0] + t.mat[1][1] + t.mat[2][2];
	if (trace > 0){
		float s = std::sqrt(trace + 1.f);
		w = s / 2;
		s = 0.5 / s;
		v.x = (t.mat[2][1] - t.mat[1][2]) * s;
		v.y = (t.mat[0][2] - t.mat[2][0]) * s;
		v.z = (t.mat[1][0] - t.mat[0][1]) * s;
	}
	else {
		const std::array<int, 3> next{1, 2, 0};
		std::array<float, 3> q;
		int i = 0;
		if (t.mat[1][1] > t.mat[0][0]){
			i = 1;
		}
		if (t.mat[2][2] > t.mat[i][i]){
			i = 2;
		}
		int j = next[i];
		int k = next[j];
		float s = std::sqrt((t.mat[i][i] - (t.mat[j][j] + t.mat[k][k])) + 1);
		q[i] = s * 0.5;
		if (s != 0){
			s = 0.5 / s;
		}
		w = (t.mat[k][j] - t.mat[j][k]) * s;
		q[j] = (t.mat[j][i] + t.mat[i][j]) * s;
		q[k] = (t.mat[k][i] + t.mat[i][k]) * s;
		v = Vector{q[0], q[1], q[2]};
	}
}
Quaternion Quaternion::slerp(const Quaternion &q, float t) const {
	float cos_theta = dot(q);
	if (cos_theta > 0.9995){
		return ((1 - t) * *this + t * q).normalized();
	}
	float theta = std::acos(clamp(cos_theta, -1.f, 1.f));
	Quaternion q_perp = (q - *this * cos_theta).normalized();
	return *this * std::cos(theta * t) + q_perp * std::sin(theta * t);
}
Transform Quaternion::to_transform() const {
	float xx = v.x * v.x, yy = v.y * v.y, zz = v.z * v.z,
		  xy = v.x * v.y, xz = v.x * v.z, yz = v.y * v.z,
		  wx = v.x * w, wy = v.y * w, wz = v.z * w;
	Matrix4 m{{
		1 - 2 * (yy + zz), 2 * (xy + wz), 2 * (xz - wy), 0,
		2 * (xy - wz), 1 - 2 * (xx + zz), 2 * (yz + wx), 0,
		2 * (xz + wy), 2 * (yz - wx), 1 - 2 * (xx + yy), 0,
		0, 0, 0, 1
	}};
	//Transpose since we're left-handed. TODO are we?
	return Transform{m.transpose(), m};
}

