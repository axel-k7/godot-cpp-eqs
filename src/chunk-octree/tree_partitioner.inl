#pragma once

template<typename T, size_t split_size>
void TreePartitioner<T, split_size>::split(TreeNode* _node) {
    if (!_node->is_leaf() || _node->depth >= depth_limit) return;

    for (int i = 0; i < split_size; ++i) {
        //initialilze child
        _node->children[i] = create_node(
            _node,
            registry.CreateChunk(element_limit + 1),
            _node->depth+1
        );

        on_create_child(_node, _node->children[i].get(), i);
    }

	auto* chunk = registry.GetChunk(_node->chunk_id);

    std::vector<size_t> element_ids;
    for (int i = 0; i < chunk->count; i++)
        element_ids.push_back(chunk->GetAt(i).id);


	for (auto& id : element_ids)
        move_entry_to_leaf(_node, id, registry.GetEntry(id));

    registry.DestroyChunk(_node->chunk_id);
    _node->chunk_id = SIZE_MAX;
    _node->pending_split = false;

    //TEMP
    print_line("split octree");
}


template<typename T, size_t split_size>
void TreePartitioner<T, split_size>::merge(TreeNode* _node) {
    if (!_node || _node->is_leaf()) return;

    _node->chunk_id = registry.CreateChunk(element_limit + 1);

    //recursively remove children
    for (int i = 0; i < split_size; ++i) {
        std::unique_ptr<TreeNode>& current_child = _node->children[i];
        merge(current_child.get());

        //move all child elements to parent
        auto child_chunk = registry.GetChunk(current_child->chunk_id);
        auto items = child_chunk->Items();

        for (int j = 0; i < items.count; ++j)
            move_entry(_node, items[0].id);

        registry.DestroyChunk(current_child->chunk_id);
        current_child.reset();
    }

    //TEMP
    print_line("merged octree");
}


template<typename T, size_t split_size>
auto TreePartitioner<T, split_size>::move_entry_to_leaf(TreeNode* _node, size_t _id, T& _data) -> TreeNode* {
    if (!node_contains(_node, _data)) return nullptr;

    if (!_node->is_leaf()) { //data within boundaries but node isn't a leaf
        int index = get_child_index(_node, _data);
	    return move_entry_to_leaf(_node->children[index].get(), _id, _data);
    }

    move_entry(_node, _id);
    return _node;
}

template<typename T, size_t split_size>
auto TreePartitioner<T, split_size>::move_entry(TreeNode* _node, size_t _id) {
    if (registry.GetChunk(_node->chunk_id)->count >= element_limit && _node->depth < depth_limit) {
        _node->pending_split = true;
        pending_splits.push_back(_node);
    }

    registry.MoveEntry(_id, _node->chunk_id);
    id_node_map[_id] = _node;
}



template<typename T, size_t split_size>
auto TreePartitioner<T, split_size>::try_populate_node(TreeNode* _node, const T& _data) -> size_t {
    if (!node_contains(_node, _data)) return Registry<T>::INVALID_ENTRY_ID;

    TreeNode* leaf = find_node_recursive(_node, _data);
    size_t id = registry.CreateEntry(leaf->chunk_id, _data);

    //just duplicating place entry logic here to avoid unnecessary MoveEntry
    if (registry.GetChunk(leaf->chunk_id)->count >= element_limit && leaf->depth < depth_limit) {
        leaf->pending_split = true;
        pending_splits.push_back(leaf);
    }
    id_node_map[id] = leaf;

    return id;
}


template<typename T, size_t split_size>
auto TreePartitioner<T, split_size>::validate_entry(size_t _id) -> bool {
    TreeNode* node = get_node(_id);
    if (!node) return false;

    T& data = registry.GetEntry(_id);
    if (node_contains(node, data)) return true;

    TreeNode* parent = node->parent;
    while (parent) {
        if (node_contains(parent, data))
            break;

        parent = parent->parent;
    }

    //exited tree bounds -> destroy data
    //maybe shouldn't?
    if (!parent) {
        registry.DestroyEntry(_id);
        return false;
    }

    move_entry_to_leaf(parent, _id, data);
    handle_splits();

    return true;
}


template<typename T, size_t split_size>
void TreePartitioner<T, split_size>::handle_splits() {
    while(!pending_splits.empty()) {
        TreeNode* node = pending_splits.back();
        pending_splits.pop_back();
        split(node);
    }
}


template<typename T, size_t split_size>
auto TreePartitioner<T, split_size>::find_node_recursive(TreeNode* _node, const T& _data) -> TreeNode* {
    if (!node_contains(_node, _data)) return nullptr;

    if (_node->is_leaf()) return _node;

    int index = get_child_index(_node, _data);
    return find_node_recursive(_node->children[index].get(), _data);
}


template<typename T, size_t split_size>
auto TreePartitioner<T, split_size>::get_node(size_t _entry_id) -> TreeNode* {
    auto it = id_node_map.find(_entry_id);
    return it != id_node_map.end() ? it->second : nullptr;
}


template<typename T, size_t split_size>
void TreePartitioner<T, split_size>::remove_entry(size_t _id) {
    registry.DestroyEntry(_id);
    id_node_map.erase(_id);
}


template<typename T, size_t split_size>
auto TreePartitioner<T, split_size>::try_insert(const T& _data) -> size_t {
    size_t id = try_populate_node(root_node.get(), _data);

    handle_splits();
    return id;
}
