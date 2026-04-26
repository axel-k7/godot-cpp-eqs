#pragma once

#include <memory>
#include <unordered_map>

#include "sdf_evaluator.h"

class Octri {
protected:
	struct OctriNode {
		OctriNode(AABB _bounds, int _depth)
			: bounds(_bounds)
    		, depth(_depth)
		{}

		std::unique_ptr<OctriNode> children[8];
		std::vector<int> tri_ids;

		AABB bounds;
		int depth;

		bool is_leaf() const { return children[0] == nullptr; }
	};


	std::vector<Triangle> triangles;
	std::unique_ptr<OctriNode> root_node;

	int triangle_limit;
	int depth_limit;

	void try_insert(OctriNode* _node, int _index);
	void split(OctriNode* _node);
	auto get_node(const Vector3& _point) const -> OctriNode*;

	void get_closet_dist_recursive(const OctriNode* _node, const Vector3& _point, float& _min_dist) const;

public:
	Octri(AABB _bounds, int _depth_limit, int _triangle_limit)
		: depth_limit(_depth_limit)
		, triangle_limit(_triangle_limit)
	{
		root_node = std::make_unique<OctriNode>( _bounds, 0 );
	}

	void build_tree(const std::vector<Triangle>& _triangles);
	auto get_closest_distance(const Vector3& _point) const -> float;
};
