#ifndef KD_POINT_TREE_H
#define KD_POINT_TREE_H

#include <type_traits>
#include "linalg/point.h"

/*
 * Walter Brown's void_t type and CWG 1558 workaround for detecting members of
 * the type being stored in the K-d tree since we need to check that the type has member
 * named position that is a Point type we can work with to build the tree
 */
template<typename> struct voider { using type = void; };
template<typename T> using void_t = typename voider<T>::type;
template<typename, typename = void> struct has_position_member : std::false_type {};
template<typename T> struct has_position_member<T, void_t<decltype(T::position)>> : std::is_same<Point, decltype(T::position)> {};

/*
 * A K-d tree specialized for storing point data, implemented similar
 * to the KdTree in PBRT. Can store any type that has a position member
 * that is of type Point and performs k-nearest queries on the data
 */
template<typename T>
class KdPointTree {
	static_assert(has_position_member<T>::value, "Type to build KdPointTree around must have an public member named position of type Point");
};

#endif

