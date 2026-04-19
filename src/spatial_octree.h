#pragma once


#include "godot_cpp/variant/aabb.hpp"

#include "debug_drawer.h"
#include "tree_partitioner.h"
#include "sdf_evaluator.h"

using namespace godot;

template<typename T>
class SpatialOctree : public TreePartitioner<T, 8> {
protected:
	using TreeNode = typename TreePartitioner<T, 8>::TreeNode;
	struct OctreeNode : TreeNode {
		OctreeNode(TreeNode* _parent, size_t _chunk_id, int _depth, AABB _bounds)
			: TreeNode(_parent, _chunk_id, _depth)	
			, bounds(_bounds)
		{}

		AABB bounds;
	};

	AABB root_bounds;

	virtual auto get_position(const T& _data) -> Vector3 = 0;

	auto get_child_index(TreeNode* _parent, const T& _data) -> int override;
	auto node_contains(TreeNode* _parent, const T& _data) -> bool override;
	auto create_node(TreeNode* _parent, size_t _chunk_id, int _depth) -> std::unique_ptr<TreeNode> override;
	void on_create_child(TreeNode* _parent, TreeNode* _child, int _index) override;
	
	
public:
	SpatialOctree(int _element_limit, int _depth_limit, AABB _bounds)
		: TreePartitioner<T, 8>(_element_limit, _depth_limit)
		, root_bounds(_bounds)
	{}

	void init() override;
	
	void draw_debug(Color _color);
	void draw_debug_recursive(TreeNode* _node, Color _color);	
};

#include "spatial_octree.inl"