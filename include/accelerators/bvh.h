#ifndef BVH_H
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
	/*
	 * Nodes used to store the final flattened BVH structure, the first child
	 * is right after the parent but the second child is at some offset further down
	 */
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
	 * can be stored per node, default is 128, max is 256
	 * The defaults for the empty constructor will build an empty BVH
	 */
	BVH(const std::vector<Geometry*> &geom = std::vector<Geometry*>{},
		SPLIT_METHOD split = SPLIT_METHOD::SAH, unsigned max_geom = 128);
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
	/*
	 * Build a leaf node in the tree using the geometry passed and push the ordered geometry for the
	 * leaf into ordered_geom
	 */
	std::unique_ptr<BuildNode> build_leaf(std::vector<GeomInfo> &build_geom, std::vector<Geometry*> &ordered_geom,
		int start, int end, const BBox &box);
	/*
	 * Recursively flatten the BVH tree into the flat nodes vector
	 * offset tracks the current offset into the flat nodes vector
	 */
	uint32_t flatten_tree(const std::unique_ptr<BuildNode> &node, uint32_t &offset);
	/*
	 * A specialized fast bbox intersection test for the BVH traversal
	 */
	bool fast_box_intersect(const BBox &bounds, Ray &r, const Vector &inv_dir, const std::array<int, 3> &neg_dir) const;
};

#endif

