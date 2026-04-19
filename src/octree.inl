#include "octree.h"


template<typename T>
auto Octree<T>::try_insert(T& _data) -> size_t {
    size_t id = try_populate_node(root_node.get(), _data);
    
    while(!pending_splits.empty()) {
        OctreeNode* node = pending_splits.back();
        pending_splits.pop_back();
        split(node);   
    }

    return id;
}

template<typename T>
auto Octree<T>::try_populate_node(OctreeNode* _node, T& _data) -> size_t {
    if (!node_contains(_node, _data)) return Registry<T>::INVALID_ENTRY_ID;

    if (_node->is_leaf()) {
        if (registry.GetChunk(_node->chunk_id)->count >= element_limit && _node->depth < depth_limit)
            pending_splits.push_back(_node);
        
        return registry.CreateEntry(_node->chunk_id, _data);
    }

	int index = get_child_index(_node, _data);
	return try_populate_node(_node->children[index].get(), _data);
}


template<typename T>
void Octree<T>::split(OctreeNode* _node) {
    if (!_node->is_leaf() || _node->depth >= depth_limit) return;

    for (int i = 0; i < 8; ++i) {
        //initialilze child
        _node->children[i] = std::make_unique<OctreeNode>(
            registry.CreateChunk(element_limit + 1),
            _node->depth+1
        );

        on_create_child(_node, _node->children[i].get(), i);
    }

	auto* chunk = registry.GetChunk(_node->chunk_id);

    std::vector<size_t> element_ids;
    for (int i = 0; i < chunk->count; i++)
        element_ids.push_back(chunk->GetAt(i).id);
    

	for (auto& id : element_ids) {
		int index = get_child_index(_node, registry.GetEntry(id));
		registry.MoveEntry(id, _node->children[index]->chunk_id);
	}


    registry.DestroyChunk(_node->chunk_id);
    _node->chunk_id = SIZE_MAX;

    //TEMP
    print_line("split octree");
}

template<typename T>
void Octree<T>::merge(OctreeNode* _node) {
    if (!_node || _node->is_leaf()) return;
    
    _node->chunk_id = registry.CreateChunk(element_limit + 1);

    //recursively remove children
    for (int i = 0; i < 8; ++i) {
        std::unique_ptr<OctreeNode>& current_child = _node->children[i];
        merge(current_child.get());

        //move all child elements to parent
        auto child_chunk = registry.GetChunk(current_child->chunk_id);
        auto items = child_chunk->Items();

        for (int i = 0; i < items.count; i++)
            registry.MoveEntry(items[0].id, _node->chunk_id);

        registry.DestroyChunk(current_child->chunk_id);
        current_child.reset();
    }

    //TEMP
    print_line("merged octree");
}


template<typename T>
auto Octree<T>::get_node_recursive(OctreeNode* _node, T& _data) -> OctreeNode* {
    if (!node_contains(_node, _data)) return nullptr;

    if (_node->is_leaf()) return _node;

    int index = get_child_index(_node, _data);

    return get_node_recursive(_node->children[index].get(), _data);
}



/*
template<typename T>
void Octree<T>::draw_debug(Color _color) {
    if (!root_node) return;

    draw_debug_recursive(root_node.get(), _color);

}

template<typename T>
void Octree<T>::draw_debug_recursive(OctreeNode* _node, Color _color) {
    if (!_node) return;

    DebugDrawer::get_singleton()->draw_aabb(_node->bounds, _color);

    if (_node->is_leaf()) return;

    for (int i = 0; i < 8; ++i)
        draw_debug_recursive(_node->children[i].get(), _color);
}
*/