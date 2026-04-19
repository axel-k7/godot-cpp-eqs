#pragma once

#include "godot_cpp/variant/aabb.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "debug_drawer.h"
#include "registry.h"

using namespace godot;


template<typename T>
class Octree {
protected:
	struct OctreeNode {
		OctreeNode(size_t _chunk_id, int _depth)
			: chunk_id(_chunk_id)
    		, depth(_depth)
		{}

		std::unique_ptr<OctreeNode> children[8];
		size_t chunk_id;
		int depth;

		bool is_leaf() const { return children[0] == nullptr; }
	};

	Registry<T> registry;
	
	std::vector<OctreeNode*> pending_splits;
	std::unique_ptr<OctreeNode> root_node;
	
	int depth_limit;
	int element_limit;
	
	void split(OctreeNode* _node);
	void merge(OctreeNode* _node);

	
	auto try_populate_node(OctreeNode* _node, T& _data) -> size_t;
	
	virtual auto get_child_index(OctreeNode* _parent, const T& _data) -> int = 0;
	virtual auto node_contains(OctreeNode* _parent, const T& _data) -> bool = 0;
	virtual void on_create_child(OctreeNode* _parent, OctreeNode* _child, int _index) = 0;
	
public:
	Octree(int _element_limit, int _depth_limit) 
	: element_limit(_element_limit)
	, depth_limit(_depth_limit)
	{ }
	
	auto try_insert(T& _data) -> size_t;
	//void draw_debug(Color _color);

	auto find_node(T& _data) -> OctreeNode* { return get_node_recursive(root_node.get(), _data); };
	
	virtual void init() = 0;



protected:
	auto get_node_recursive(OctreeNode* _node, T& _data) -> OctreeNode*;
	//void draw_debug_recursive(OctreeNode* _node, Color _color);
};

#include "octree.inl"
