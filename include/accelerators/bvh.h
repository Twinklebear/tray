#ifndef BHV_H
#define BVH_H

#include <memory>
#include <vector>
#include <array>
#include "linalg/ray.h"
#include "linalg/util.h"
#include "geometry/geometry.h"
#include "geometry/bbox.h"

/*
 * Different methods that can be used to partition the space
 * when building the BVH
 */
enum class SPLIT_METHOD { MIDDLE, EQUAL, SAH };

/*
 * A BVH built off of the one introduced in PBR
 */
class BVH {
	//Information about some geometry being place in the BVH
	struct GeomInfo {
		//The index of the geometry in the BVH's geometry vector
		int geom_idx;
		//Center of the geometry's bounding box
		Point center;
		BBox bounds;

		GeomInfo(int i, const BBox &b);
	};
	//Node used when constructing the BVH tree
	struct BuildNode {
		//Node's children, null if a leaf node
		std::array<std::unique_ptr<BuildNode>, 2> children;
		//Offset to the first geometry in this node and # of geometry in it
		//ngeom = 0 if this is an interior node
		int geom_offset, ngeom;
		BBox bounds;
		//Which axis was split to form this node's children
		AXIS split;

		/*
		 * Construct a leaf node
		 */
		BuildNode(int geom_offset, int ngeom, const BBox &bounds);
		/*
		 * Construct an interior node, where a & b are the node's children
		 */
		BuildNode(AXIS split, std::unique_ptr<BuildNode> a, std::unique_ptr<BuildNode> b);
	};
	//Nodes used to store the final flattened BVH structure
	struct FlatNode {
		BBox bounds;
		union {
			//Used for leaves to locate geometry
			int geom_offset;
			//Used for interiors to locate second child
			int second_child;
		};
		//Number of geometry stored in this node, 0 if interior
		uint16_t ngeom;
		uint16_t axis;
	};
	//Bucket used for SAH split method
	struct SAHBucket {
		int count;
		BBox bounds;

		SAHBucket();
	};

	SPLIT_METHOD split;
	unsigned max_geom;
	//The geometry being stored in this BVH
	std::vector<Geometry*> geometry;
	//The final flatted BVH structure
	std::vector<FlatNode> flat_nodes;

public:
	/*
	 * Construct the BVH to create a hierarchy of the refined geometry passed in
	 * using the desird split method. max_geom specifies the maximum geometry that
	 * can be stored per node, default is 32, max is 128
	 */
	BVH(const std::vector<Geometry*> &geom, SPLIT_METHOD split, unsigned max_geom = 32);
	/*
	 * Get the bounds for the BVH
	 */
	BBox bounds() const;
	/*
	 * Perform an intersection test on the geometry stored in the BVH
	 */
	bool intersect(Ray &r, HitInfo &hitinfo);

private:
	/*
	 * Construct a subtree of the BVH for the build_geom from [start, end)
	 * and return the root of this subtree. The geometry is placed in the child
	 * nodes is partitioned into ordered geom for easier look up later
	 * Also returns the total nodes in this subtree, for use later when flattening
	 * the BVH
	 */
	std::unique_ptr<BuildNode> build(std::vector<GeomInfo> &build_geom, std::vector<Geometry*> &ordered_geom,
		int start, int end, int &total_nodes);
};

#endif

