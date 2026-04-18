#include "octree.h"

void Octree::_ready() {
    init();
}

void Octree::init() {
    AABB root_bounds(get_global_position(), Vector3(base_size, base_size, base_size));

    root_node = std::make_unique<OctreeNode>(
        registry.CreateChunk(element_limit),
        root_bounds,
        0
    );

    print_line("initialized octree");
}


auto Octree::try_insert(Vector3 _point) -> size_t {
    print_line("tried inserting point (C++)");

    OctreeNode* node = find_node(_point);
    if (!node) return SIZE_MAX;

    size_t id = registry.CreateEntry(node->chunk_id, InfluencePoint{_point});
    
    if (registry.GetChunk(node->chunk_id)->count >= element_limit)
        split(node);

    return id;
}


auto Octree::find_node(Vector3 _point) -> OctreeNode* {
    if (!root_node->bounds.has_point(_point)) return nullptr;

    return find_node_recursive(root_node.get(), _point);
}

auto Octree::find_node_recursive(OctreeNode* _node, Vector3 _point) -> OctreeNode* {
    if (!_node->bounds.has_point(_point)) return nullptr;

    if (_node->children[0]) {
        for (int i = 0; i < 8; ++i) {
            OctreeNode* node = find_node_recursive(_node->children[i].get(), _point);
            if (node)
                return node;
        }
    }

    return _node;
}


void Octree::split(OctreeNode* _node) {
    if (_node->children[0] || _node->depth >= depth_limit) return; //if node already has children or at limit
    
    Vector3 half_size = _node->bounds.size * 0.5f;

    for (int i = 0; i < 8; ++i) {
        Vector3 offset(
            (i & 1) ? half_size.x : 0.f,
            (i & 2) ? half_size.y : 0.f,
            (i & 4) ? half_size.z : 0.f
        );
        //(000) = left, bottom, front
        //(111) = right, top, back

        //initialilze child
        _node->children[i] = std::make_unique<OctreeNode>(
            registry.CreateChunk(element_limit),
            AABB(_node->bounds.position + offset, half_size),
            _node->depth+1
        );
    }

    auto chunk = registry.GetChunk(_node->chunk_id);
    auto elements = chunk->GetAll();
    for (int i = 0; i < chunk->count; i++) {
        OctreeNode* target_child = find_node_recursive(_node, elements[i].data.position);
        registry.MoveEntry(elements[0].id, target_child->chunk_id);
    }

    registry.DestroyChunk(_node->chunk_id);
    _node->chunk_id = SIZE_MAX;

    //TEMP
    print_line("split octree");
}

void Octree::merge(OctreeNode* _node) {
    if (!_node || !_node->children[0]) return;
    
    _node->chunk_id = registry.CreateChunk(element_limit);

    //recursively remove children
    for (int i = 0; i < 8; ++i) {
        std::unique_ptr<OctreeNode>& current_child = _node->children[i];
        merge(current_child.get());
        
        //move all child elements to parent
        auto child_chunk = registry.GetChunk(current_child->chunk_id);
        auto elements = child_chunk->GetAll();

        for (int i = 0; i < child_chunk->count; i++)
            registry.MoveEntry(elements[0].id, _node->chunk_id);

        registry.DestroyChunk(current_child->chunk_id);
        current_child.reset();
    }

    //TEMP
    print_line("merged octree");
}



void Octree::draw_debug(Color _color) {
    if (!root_node) return;

    draw_debug_recursive(root_node.get(), _color);

}

void Octree::draw_debug_recursive(OctreeNode* _node, Color _color) {
    if (!_node) return;

    DebugDrawer::get_singleton()->draw_aabb(_node->bounds, _color);

    if (!_node->children[0]) return;

    for (int i = 0; i < 8; ++i)
        draw_debug_recursive(_node->children[i].get(), _color);
}