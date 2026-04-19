#include "spatial_octree.h"

template<typename T>
auto SpatialOctree<T>::get_child_index(TreeNode* _parent, const T& _data) -> int {
    OctreeNode* node = static_cast<OctreeNode*>(_parent);
    Vector3 position = get_position(_data);
    Vector3 center = node->bounds.position + node->bounds.size*0.5;
        
    int index = 0;
    if (position.x > center.x) index |= 1;
    if (position.y > center.y) index |= 2;
    if (position.z > center.z) index |= 4;

    return index;
}
	

template<typename T>
auto SpatialOctree<T>::node_contains(TreeNode* _parent, const T& _data) -> bool {
    return static_cast<OctreeNode*>(_parent)->bounds.has_point(get_position(_data));
}


template<typename T>
auto SpatialOctree<T>::create_node(TreeNode* _parent, size_t _chunk_id, int _depth) -> std::unique_ptr<TreeNode> {
    return std::make_unique<OctreeNode>(_parent, _chunk_id, _depth, AABB());
}


template<typename T>
void SpatialOctree<T>::on_create_child(TreeNode* _parent, TreeNode* _child, int _index) {
    OctreeNode* parent = static_cast<OctreeNode*>(_parent);
    OctreeNode* child = static_cast<OctreeNode*>(_child);

    Vector3 half_size = parent->bounds.size * 0.5f;
    Vector3 offset(
        (_index & 1) ? half_size.x : 0.f,
        (_index & 2) ? half_size.y : 0.f,
        (_index & 4) ? half_size.z : 0.f
    );

    child->bounds = AABB(parent->bounds.position + offset, half_size);
}

	
template<typename T>
void SpatialOctree<T>::init() {
    root_node = create_node(
        nullptr,
        registry.CreateChunk(element_limit + 1),
        0
    );

    static_cast<OctreeNode*>(root_node.get())->bounds = root_bounds;
}


template<typename T>
void SpatialOctree<T>::draw_debug(Color _color) {
    if (!root_node) return;

    draw_debug_recursive(root_node.get(), _color);

}

template<typename T>
void SpatialOctree<T>::draw_debug_recursive(TreeNode* _node, Color _color) {
    if (!_node) return;

    OctreeNode* node = static_cast<OctreeNode*>(_node);

    DebugDrawer::get_singleton()->draw_aabb(node->bounds, _color);

    if (node->is_leaf()) return;

    for (int i = 0; i < 8; ++i)
        draw_debug_recursive(node->children[i].get(), _color);
}
