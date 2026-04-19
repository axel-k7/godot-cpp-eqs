#pragma once

#include <memory>
#include <unordered_map>

#include "registry.h"

template<typename T, size_t split_size>
class TreePartitioner {
protected:
	struct TreeNode {
		TreeNode(TreeNode* _parent, size_t _chunk_id, int _depth)
			: parent(_parent)
			, chunk_id(_chunk_id)
    		, depth(_depth)
		{}

		std::unique_ptr<TreeNode> children[split_size];
		TreeNode* parent; //need parent pointer to validate if element still exists in tree
		size_t chunk_id;
		int depth;

		bool pending_split = false;

		bool is_leaf() const { return children[0] == nullptr; }
	};

	Registry<T> registry;
	
	std::unordered_map<size_t, TreeNode*> id_node_map;

	std::vector<TreeNode*> pending_splits;
	std::unique_ptr<TreeNode> root_node;
	
	int depth_limit;
	int element_limit;
	
	void split(TreeNode* _node);
	void merge(TreeNode* _node);
	
	auto try_populate_node(TreeNode* _node, const T& _data) -> size_t;
	
	auto get_node(size_t _entry_id) -> TreeNode*;

	auto find_node(const T& _data) -> TreeNode* { return find_node_recursive(root_node.get(), _data); };
	auto find_node_recursive(TreeNode* _node, const T& _data) -> TreeNode*;

	auto move_entry(TreeNode* _node, size_t _id);
	auto move_entry_to_leaf(TreeNode* _start, size_t _id, T& _data) -> TreeNode*;
	
	void handle_splits();
	
	//determine which child should hold _data
	virtual auto get_child_index(TreeNode* _parent, const T& _data) -> int = 0;
	//if data is within the defined boundaries of a node
	virtual auto node_contains(TreeNode* _parent, const T& _data) -> bool = 0;
	virtual auto create_node(TreeNode* _parent, size_t _chunk_id, int _depth) -> std::unique_ptr<TreeNode> = 0;
	virtual void on_create_child(TreeNode* _parent, TreeNode* _child, int _index) = 0;
	
public:
	TreePartitioner(int _element_limit, int _depth_limit) 
	: element_limit(_element_limit)
	, depth_limit(_depth_limit)
	{ }
	
	auto try_insert(const T& _data) -> size_t;
	void remove_entry(size_t _id);
	auto validate_entry(size_t _id) -> bool;
	auto get_entry(size_t _id) -> T& { return registry.GetEntry(_id); }
	
	//create root node here
	virtual void init() = 0;
};

#include "tree_partitioner.inl"
