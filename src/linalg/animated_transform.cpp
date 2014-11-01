#include "linalg/animated_transform.h"

AnimatedTransform::AnimatedTransform(const Transform &start_transform, float start_time,
	const Transform &end_transform, float end_time)
	: start_time(start_time), end_time(end_time), animated(start_transform != end_transform),
	start_transform(start_transform), end_transform(end_transform)
{
	decompose(start_transform.mat, translation[0], rotation[0], scaling[0]);
	decompose(end_transform.mat, translation[1], rotation[1], scaling[1]);
}
Transform AnimatedTransform::interpolate(float t) const {
	Transform transform;
	interpolate(t, transform);
	return transform;
}
void AnimatedTransform::interpolate(float t, Transform &transform) const {
	if (!animated || t <= start_time){
		transform = start_transform;
	}
	if (t >= end_time){
		transform = end_transform;
	}
	float dt = (t - start_time) / (end_time - start_time);
	Vector trans = (1 - dt) * translation[0] + dt * translation[1];
	Quaternion rot = rotation[0].slerp(rotation[1], dt);
	Matrix4 scale;
	for (int i = 0; i < 3; ++i){
		for (int j = 0; j < 3; ++j){
			scale[i][j] = lerp(dt, scaling[0][i][j], scaling[1][i][j]);
		}
	}
	transform = Transform::translate(trans) * rot.to_transform() * Transform{scale};
}
BBox AnimatedTransform::motion_bound(const BBox &b) const {
	if (!animated){
		return start_transform.inverse()(b);
	}
	BBox ret;
	for (int i = 0; i < 128; ++i){
		float time = lerp(i / 127.f, start_time, end_time);
		Transform t = interpolate(time);
		ret = ret.box_union(t(b));
	}
	return ret;
}
Point AnimatedTransform::operator()(float t, const Point &p) const {
	Point out;
	(*this)(t, p, out);
	return out;
}
void AnimatedTransform::operator()(float t, const Point &p, Point &out) const {
	interpolate(t)(p, out);
}
Vector AnimatedTransform::operator()(float t, const Vector &v) const {
	Vector out;
	(*this)(t, v, out);
	return out;
}
void AnimatedTransform::operator()(float t, const Vector &v, Vector &out) const {
	interpolate(t)(v, out);
}
Ray AnimatedTransform::operator()(const Ray &r) const {
	Ray out = r;
	(*this)(r, out);
	return out;
}
void AnimatedTransform::operator()(const Ray &r, Ray &out) const {
	interpolate(r.time)(r, out);
}
RayDifferential AnimatedTransform::operator()(const RayDifferential &r) const {
	RayDifferential out = r;
	(*this)(r, out);
	return out;
}
void AnimatedTransform::operator()(const RayDifferential &r, RayDifferential &out) const {
	interpolate(r.time)(r, out);
}
void AnimatedTransform::decompose(const Matrix4 &m, Vector &trans, Quaternion &rot, Matrix4 &scale){
	//Extract translation component and remove it from the matrix to compute scaling and rotation
	trans = Vector{m[0][3], m[1][3], m[2][3]};
	Matrix4 mat = m;
	for (int i = 0; i < 3; ++i){
		mat[i][3] = 0;
		mat[3][i] = 0;
	}
	mat[3][3] = 1;
	//Use polar decomposition to extract R and S components
	float norm = 0;
	Matrix4 r = mat;
	for (int i = 0; i < 100 && norm > 0.001; ++i){
		Matrix4 r_next = 0.5 * (r + r.transpose().inverse());
		norm = 0;
		for (int i = 0; i < 3; ++i){
			float n = std::abs(r[i][0] - r_next[i][0])
				+ std::abs(r[i][1] - r_next[i][1])
				+ std::abs(r[i][2] - r_next[i][2]);
			norm = std::max(norm, n);
		}
		r = r_next;
	}
	rot = Quaternion{r};
	scale = r.inverse() * mat;
}

