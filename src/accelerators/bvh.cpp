#include <algorithm>
#include <memory>
#include <cmath>
#include <vector>
#include <array>
#include "linalg/ray.h"
#include "linalg/util.h"
#include "geometry/geometry.h"
#include "geometry/bbox.h"
#include "accelerators/bvh.h"

BVH::GeomInfo::GeomInfo(int i, const BBox &b) : geom_idx(i), center(b.lerp(0.5, 0.5, 0.5)), bounds(b){}

BVH::BuildNode::BuildNode(int geom_offset, int ngeom, const BBox &bounds)
	: children({nullptr, nullptr}), geom_offset(geom_offset), ngeom(ngeom), bounds(bounds)
{}
BVH::BuildNode::BuildNode(AXIS split, std::unique_ptr<BuildNode> a, std::unique_ptr<BuildNode> b)
	: children({std::move(a), std::move(b)}), geom_offset(0), ngeom(0),
	bounds(children[0]->bounds.box_union(children[1]->bounds)), split(split)
{}

BVH::SAHBucket::SAHBucket() : count(0){}

BVH::BVH(const std::vector<Geometry*> &geom, SPLIT_METHOD split, unsigned max_geom)
	: split(split), max_geom(std::min(128u, max_geom))
{
	for (Geometry *g : geom){
		g->refine(geometry);
	}
	//If we didn't get any geometry to partition then we don't have to do anything
	if (geometry.empty()){
		return;
	}

	//Get bounds and index info together for the geometry we're storing
	std::vector<GeomInfo> build_geom;
	build_geom.reserve(geometry.size());
	for (size_t i = 0; i < geometry.size(); ++i){
		build_geom.emplace_back(i, geometry[i]->bound());
	}

	std::vector<Geometry*> ordered_geom;
	ordered_geom.reserve(geometry.size());
	int total_nodes = 0;
	std::unique_ptr<BuildNode> root = build(build_geom, ordered_geom, 0, geometry.size(), total_nodes);
	//Our BVH structure refers to the ordered geometry so swap out our unordered list for
	//the correctly ordered one
	geometry.swap(ordered_geom);

	//Flatten the tree
	
	//Delete the node structured tree since we no longer need it
}
BBox BVH::bounds() const {
}
bool BVH::intersect(Ray &r, HitInfo &hitinfo){
}
std::unique_ptr<BVH::BuildNode> BVH::build(std::vector<GeomInfo> &build_geom, std::vector<Geometry*> &ordered_geom,
	int start, int end, int &total_nodes)
{
	++total_nodes;
	//Find total bounds for the geometry we're trying to store
	BBox box;
	for (int i = start; i < end; ++i){
		box = box.box_union(build_geom[i].bounds);
	}
	int ngeom = end - start;
	//Build and return a leaf node for the geometry
	if (ngeom == 1){
		//Store the offset to this leaf's geometry then push it on
		int geom_offset = ordered_geom.size();
		//TODO: don't we only need to push on start? since we know end - start = 1 at this point
		for (int i = start; i < end; ++i){
			ordered_geom.push_back(geometry[build_geom[i].geom_idx]);
		}
		return std::unique_ptr<BuildNode>{new BuildNode{geom_offset, ngeom, box}};
	}
	//Need to build an interior node
	//Figure out which axis to split on by bounding the various geometry's centroids
	//and picking the one with the most variation
	BBox centroids;
	for (int i = start; i < end; ++i){
		centroids = centroids.box_union(build_geom[i].bounds);
	}
	AXIS axis = centroids.max_extent();
	//We can now partition the primitives along this axis
	int mid = (start + end) / 2;
	//If all the geometry's centers are on the same point we can't partition
	if (centroids.max[axis] == centroids.min[axis]){
		//Check that we can fit all the geometry into a single leaf node, if not we need
		//to force a split
		if (ngeom < max_geom){
			//Store the offset to this leaf's geometry then push it on
			int geom_offset = ordered_geom.size();
			for (int i = start; i < end; ++i){
				ordered_geom.push_back(geometry[build_geom[i].geom_idx]);
			}
			return std::unique_ptr<BuildNode>{new BuildNode{geom_offset, ngeom, box}};
		}
		else {
			return std::unique_ptr<BuildNode>{new BuildNode{axis,
				build(build_geom, ordered_geom, start, mid, total_nodes),
				build(build_geom, ordered_geom, mid, end, total_nodes)}};
		}
	}
	//Partition the primitives base on split method chosen
	switch (split){
		case SPLIT_METHOD::MIDDLE: {
			float mid_pt = 0.5f * (centroids.min[axis] + centroids.max[axis]);
			auto mid_ptr = std::partition(build_geom.begin() + start, build_geom.begin() + end,
				[axis, mid_pt](const GeomInfo &g){
					return g.center[axis] < mid_pt;
				});
			mid = std::distance(mid_ptr, build_geom.begin());
			//This technique can fail if the geometry bounds overlap a lot, in which case
			//fall through to equal split method
			if (mid != start && mid != end){
				break;
			}
		}
		case SPLIT_METHOD::EQUAL: {
			mid = (start + end) / 2;
			std::nth_element(build_geom.begin() + start, build_geom.begin() + mid,
				build_geom.begin() + end,
				[axis](const GeomInfo &a, const GeomInfo &b){
					return a.center[axis] < b.center[axis];
				});
			break;
		}
		case SPLIT_METHOD::SAH: {
			//If there's only a few primitives just use EQUAL and break
			if (ngeom < 5){
				mid = (start + end) / 2;
				std::nth_element(build_geom.begin() + start, build_geom.begin() + mid,
					build_geom.begin() + end,
					[axis](const GeomInfo &a, const GeomInfo &b){
						return a.center[axis] < b.center[axis];
					});
				break;
			}
		}
	}
	return std::unique_ptr<BuildNode>{new BuildNode{axis,
		build(build_geom, ordered_geom, start, mid, total_nodes),
		build(build_geom, ordered_geom, mid, end, total_nodes)}};
}

