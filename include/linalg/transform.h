#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "point.h"
#include "vector.h"
#include "ray.h"
#include "matrix4.h"
#include "geometry/bbox.h"
#include "geometry/differential_geometry.h"

/*
 * A transformation, also stores its inverse since we need it frequenctly
 */
struct Transform {
	Matrix4 mat, inv;

	Transform();
	Transform(const Matrix4 &mat);
	Transform(const Matrix4 &mat, const Matrix4 &inv);
	/*
	 * Get a translation matrix to translate by v
	 */
	static Transform translate(const Vector &v);
	/*
	 * Get a scaling matrix to scale x, y, z by the passed values
	 */
	static Transform scale(float x, float y, float z);
	/*
	 * Get a matrix rotating about x, y, z by some degrees
	 */
	static Transform rotate_x(float deg);
	static Transform rotate_y(float deg);
	static Transform rotate_z(float deg);
	/*
	 * Get a matrix rotating about an arbitrary axis by some degrees
	 */
	static Transform rotate(const Vector &axis, float deg);
	/*
	 * Get the lookat transformation matrix
	 */
	static Transform look_at(const Point &pos, const Point &center, const Vector &up);
	/*
	 * Get the perspective projection matrix
	 */
	static Transform perspective(float fov, float near, float far);
	/*
	 * Get the orthographic projection matrix
	 */
	static Transform orthographic(float near, float far);
	/*
	 * Get a transform representing the inverse of this one
	 */
	Transform inverse() const;
	/*
	 * Check if this transformation has a scaling factor in it
	 */
	bool has_scale() const;
	bool operator==(const Transform &t) const;
	bool operator!=(const Transform &t) const;
	/*
	 * Transform points, vectors, normals, rays, bboxes. Can also optionally pass
	 * the point, vector, normal, ray or bbox to store the result in instead of
	 * having it returned. It is safe if the input and output are the same object
	 */
	Point operator()(const Point &p) const;
	void operator()(const Point &in, Point &out) const;
	Vector operator()(const Vector &v) const;
	void operator()(const Vector &in, Vector &out) const;
	Normal operator()(const Normal &n) const;
	void operator()(const Normal &in, Normal &out) const;
	Ray operator()(const Ray &r) const;
	void operator()(const Ray &in, Ray &out) const;
	RayDifferential operator()(const RayDifferential &r) const;
	void operator()(const RayDifferential &in, RayDifferential &out) const;
	Matrix4 operator()(const Matrix4 &m) const;
	void operator()(const Matrix4 &in, Matrix4 &out) const;
	BBox operator()(const BBox &b) const;
	void operator()(const BBox &in, BBox &out) const;
	DifferentialGeometry operator()(const DifferentialGeometry &d) const;
	void operator()(const DifferentialGeometry &in, DifferentialGeometry &out) const;
	/*
	 * Compose transformations
	 */
	Transform operator*(const Transform &t) const;
	Transform& operator*=(const Transform &t);
	//Debug printing
	void print(std::ostream &os) const;
};
std::ostream& operator<<(std::ostream &os, const Transform &t);

#endif

