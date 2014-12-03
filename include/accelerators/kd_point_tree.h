#ifndef KD_POINT_TREE_H
#define KD_POINT_TREE_H

#include <cassert>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <vector>
#include "linalg/point.h"
#include "geometry/bbox.h"

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
template<typename P>
class KdPointTree {
	static_assert(has_position_member<P>::value, "Type to build KdPointTree around must have an public member named position of type Point");
	/*
	 * A node in the KdPointTree, stores information about its children and the split information
	 * if it's an interior node, otherwise stores flags to indicate it's a leaf
	 * Actual information for the node (eg. the point stored here) is kept in a cold array 
	 * indexed by the node id for faster traversal
	 */
	struct Node {
		float split_pos;
		//Bitfields so we can fit the struct in a cacheline
		//The left child is stored right after this one so we just need a flag, right_child
		//is the offset to the right_child, if any (if no right child this is 2^29-1)
		uint32_t split_axis:2, has_left_child:1, right_child:29;
		const static uint32_t NO_RIGHT_CHILD = (1 << 29) - 1;

		/*
		 * Initialize an interior node
		 */
		static Node interior(float split_pos, uint32_t split_axis);
		/*
		 * Initialize a leaf node. A leaf node is indicated by the split_axis being set to 3
		 */
		static Node leaf();
	};

	std::vector<P> data;
	std::vector<const P*> node_data;
	std::vector<Node> nodes;
public:
	/*
	 * Construct the KdPointTree about the set of point data passed
	 */
	KdPointTree(const std::vector<P> &points);
	KdPointTree(std::vector<P> &&points);
	/*
	 * Query the tree for a list of points within max_dist_sqr of the point, nodes within
	 * the query range are passed to the user supplied callback. Note that it is possible
	 * to modify the search radius during the query process as it will be passed by reference
	 * allowing tuning of the query as it runs
	 * the callback must define operator()(const Point&, const P&, float, float&)
	 */
	template<typename Callback>
	void query(const Point &p, float &max_dist_sqr, Callback &callback) const;
	
private:
	/*
	 * Recursively build the Kd tree on the points from [start, end), returning the index of the next free node
	 */
	uint32_t build(uint32_t node_id, int start, int end, std::vector<const P*> &build_data);
	/*
	 * Recursively query the tree looking for points that fall within the query region around the point
	 */
	template<typename Callback>
	void query(uint32_t node_id, const Point &p, float &max_dist_sqr, Callback &callback) const;
};

template<typename P>
typename KdPointTree<P>::Node KdPointTree<P>::Node::interior(float split_pos, uint32_t split_axis){
	return Node{split_pos, split_axis, 0, NO_RIGHT_CHILD};
}
template<typename P>
typename KdPointTree<P>::Node KdPointTree<P>::Node::leaf(){
	return Node{0, 3, 0, NO_RIGHT_CHILD};
}

template<typename P>
KdPointTree<P>::KdPointTree(const std::vector<P> &points) : data(points){
	nodes.resize(data.size());
	node_data.resize(data.size());
	std::vector<const P*> build_data(data.size());
	std::transform(data.begin(), data.end(), build_data.begin(), [](const auto &p){ return &p; });
	build(0, 0, data.size(), build_data);
}
template<typename P>
KdPointTree<P>::KdPointTree(std::vector<P> &&points) : data(std::move(points)){
	nodes.resize(data.size());
	node_data.resize(data.size());
	std::vector<const P*> build_data(data.size());
	std::transform(data.begin(), data.end(), build_data.begin(), [](const auto &p){ return &p; });
	build(0, 0, data.size(), build_data);
}
template<typename P>
uint32_t KdPointTree<P>::build(uint32_t node_id, int start, int end, std::vector<const P*> &build_data){
	//If we've hit the bottom make a leaf node
	if (start + 1 == end){
		nodes[node_id] = Node::leaf();
		node_data[node_id] = build_data[start];
		return node_id + 1;
	}
	//Find the longest extent of the box bounding the points and use it to split and do median split
	BBox box;
	for (auto i = build_data.begin() + start; i != build_data.begin() + end; ++i){
		box = box.box_union((*i)->position);
	}
	uint32_t split_axis = box.max_extent();
	int median = (start + end) / 2;
	//If the positions are equivalent we fall back to comparing the pointer values to arbitrarily break ties
	std::nth_element(build_data.begin() + start, build_data.begin() + median, build_data.begin() + end,
		[=](const auto *a, const auto *b){
			return a->position[split_axis] == b->position[split_axis] ? a < b : a->position[split_axis] < b->position[split_axis];
		});
	//Create this node as an interior and recursively build the left/right children
	nodes[node_id] = Node::interior(build_data[median]->position[split_axis], split_axis);
	node_data[node_id] = build_data[median];
	uint32_t next_free = node_id + 1;
	if (start < median){
		nodes[node_id].has_left_child = 1;
		next_free = build(next_free, start, median, build_data);
	}
	if (median + 1 < end){
		nodes[node_id].right_child = next_free;
		next_free = build(nodes[node_id].right_child, median + 1, end, build_data);
	}
	return next_free;
}
template<typename P>
template<typename Callback>
void KdPointTree<P>::query(const Point &p, float &max_dist_sqr, Callback &callback) const {
	assert(!nodes.empty());
	query(0, p, max_dist_sqr, callback);
}
template<typename P>
template<typename Callback>
void KdPointTree<P>::query(uint32_t node_id, const Point &p, float &max_dist_sqr, Callback &callback) const {
	const Node &node = nodes[node_id];
	float dist_sqr = 0;
	if (node.split_axis != 3){
		//Traverse the side of the tree that the query point is in first
		dist_sqr = (p[node.split_axis] - node.split_pos) * (p[node.split_axis] - node.split_pos);
		if (p[node.split_axis] <= node.split_pos){
			if (node.has_left_child){
				query(node_id + 1, p, max_dist_sqr, callback);
			}
			//If the query region also overlaps the other side query there as well
			if (dist_sqr < max_dist_sqr && node.right_child != Node::NO_RIGHT_CHILD){
				query(node.right_child, p, max_dist_sqr, callback);
			}
		}
		else {
			if (node.right_child != Node::NO_RIGHT_CHILD){
				query(node.right_child, p, max_dist_sqr, callback);
			}
			//If the query region also overlaps the other side query there as well
			if (dist_sqr < max_dist_sqr && node.has_left_child){
				query(node_id + 1, p, max_dist_sqr, callback);
			}
		}
	}
	//If the node is within dist_sqr of the point then the distance from the point to
	//the splitting axis must also be < max_dist_sqr
	if (dist_sqr < max_dist_sqr){
		dist_sqr = node_data[node_id]->position.distance_sqr(p);
		if (dist_sqr < max_dist_sqr){
			callback(p, *node_data[node_id], dist_sqr, max_dist_sqr);
		}
	}
}

#endif

