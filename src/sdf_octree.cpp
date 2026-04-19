#include "sdf_octree.h"
/*
void SDFOctree::_ready() {
    init();
}

void SDFOctree::init() {
    Vector3 size = Vector3(base_size, base_size, base_size);
    AABB root_bounds(get_global_position() - (size*0.5f), size);

    root_node = std::make_unique<OctreeNode>(
        registry.CreateChunk(element_limit + 1),
        root_bounds,
        0
    );

    print_line("initialized octree");
}


auto SDFOctree::try_insert(Triangle _tri) -> bool {
    bool success = try_populate_node(root_node.get(), _tri);

    while(!pending_splits.empty()) {
        OctreeNode* node = pending_splits.back();
        pending_splits.pop_back();
        split(node);   
    }

    return success;
}


auto SDFOctree::try_populate_node(OctreeNode* _node, Triangle _tri) -> bool {
    if (!_node->bounds.intersects(_tri.aabb)) 
        return false;

    if (_node->is_leaf()) {
        if (registry.GetChunk(_node->chunk_id)->count >= element_limit && _node->depth < depth_limit)
            pending_splits.push_back(_node);
        
        registry.CreateEntry(_node->chunk_id, _tri);
        return true;
    }

    bool child_populated = false;
    for (int i = 0; i < 8; ++i)
        if (try_populate_node(_node->children[i].get(), _tri))
            child_populated = true;

    return child_populated;
}


void SDFOctree::split(OctreeNode* _node) {
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
            registry.CreateChunk(element_limit + 1),
            AABB(_node->bounds.position + offset, half_size),
            _node->depth+1
        );
    }

    auto chunk = registry.GetChunk(_node->chunk_id);
    
    std::vector<Triangle> elements;
    for (int i = 0; i < chunk->count; ++i)
        elements.push_back(chunk->GetAt(i).data);


    registry.DestroyChunk(_node->chunk_id);
    _node->chunk_id = SIZE_MAX;


    for (auto& element : elements)
        for (int i = 0; i < 8; ++i)
            try_populate_node(_node->children[i].get(), element);

    //TEMP
    print_line("split octree node at depth: ", _node->depth);
}

void SDFOctree::merge(OctreeNode* _node) {
    if (!_node || !_node->children[0]) return;
    
    _node->chunk_id = registry.CreateChunk(element_limit + 1);

    std::vector<Triangle> unique_tris;

    //recursively remove children
    for (int i = 0; i < 8; ++i) {
        std::unique_ptr<OctreeNode>& current_child = _node->children[i];
        merge(current_child.get());
        
        auto child_chunk = registry.GetChunk(current_child->chunk_id);
        auto elements = child_chunk->GetAll();
        
        //this merge is mega-slow but sdf shouldn't really need to merge
        for (int i = 0; i < child_chunk->count; i++) {
            //if (std::find(unique_tris.begin(), unique_tris.end(), elements[i].data) != unique_tris.end())
            //    unique_tris.push_back(elements[i].data);
        };

        registry.DestroyChunk(current_child->chunk_id);
        current_child.reset();
    }

    for (Triangle& tri : unique_tris)
        try_populate_node(_node, tri);

    //TEMP
    print_line("merged octree node at depth: ", _node->depth);
}

void SDFOctree::bake(OctreeNode* _node) {
    if (!_node->is_leaf()) {
        for (int i = 0; i < 8; ++i)
            bake(_node->children[i].get());
        
        return;
    }
    
    const auto& items = registry.GetChunk(_node->chunk_id)->Items();
    Vector3 half_size = _node->bounds.size * 0.5f;

    for (int i = 0; i < 8; ++i) {
        Vector3 offset(
            (i & 1) ? half_size.x : 0.f,
            (i & 2) ? half_size.y : 0.f,
            (i & 4) ? half_size.z : 0.f
        );

        Vector3 corner_position = _node->bounds.position + offset;

        float best = FLT_MAX;
        _node->corners[i] = sdf_query(corner_position, root_node.get(), best);
    }
}

auto SDFOctree::get_sdf(Vector3 _point) -> float {
    OctreeNode* node = find_node(_point);
    if (!node) return FLT_MAX; //outside octree

    //normalize the point so it's local to the node
    Vector3 local_point = (_point - node->bounds.position) / node->bounds.size;

    //clamp incase float error
    local_point = Math::clamp(local_point, Vector3(0,0,0), Vector3(1,1,1));

    //interpolated corner values
    float x00 = Math::lerp(node->corners[0], node->corners[1], local_point.x);
    float x01 = Math::lerp(node->corners[2], node->corners[3], local_point.x);
    float x10 = Math::lerp(node->corners[4], node->corners[5], local_point.x);
    float x11 = Math::lerp(node->corners[6], node->corners[7], local_point.x);

    float y0 = Math::lerp(x00, x10, local_point.y);
    float y1 = Math::lerp(x01, x11, local_point.y);

    return Math::lerp(y0, y1, local_point.z);
}

auto SDFOctree::brute_force_sdf(Vector3 _point) -> float {
    float min_dist = FLT_MAX;
    for (auto* chunk : registry.GetAllChunks()) {
        auto& items = chunk->Items();

        for (auto& [tri, _] : items) {
             float distance = get_tri_point_dist(_point, tri);
             if (distance < min_dist)
                min_dist = distance;
        }
    }
    return min_dist;
}

auto SDFOctree::find_node(Vector3 _point) -> OctreeNode* {
    if (!root_node->bounds.has_point(_point)) return nullptr;

    return find_node_recursive(_point, root_node.get());
}

auto SDFOctree::find_node_recursive(Vector3 _point, OctreeNode* _node) -> OctreeNode* {
    if (!_node->bounds.has_point(_point)) return nullptr;

    if (_node->children[0]) {
        Vector3 center = _node->bounds.position + _node->bounds.size*0.5;
        
        int index = 0;
        if (_point.x > center.x) index |= 1;
        if (_point.y > center.y) index |= 2;
        if (_point.z > center.z) index |= 4;

        return find_node_recursive(_point, _node->children[index].get());
    }

    return _node;
}

auto SDFOctree::get_tri_point_dist(Vector3 _point, Triangle _tri) -> float {
    Vector3 closest_point;
    float min_dist_sq = FLT_MAX;

    Vector3 P = _point;
    Vector3 A = _tri.vertices[0];
    Vector3 B = _tri.vertices[1];
    Vector3 C = _tri.vertices[2];

    Vector3 N = (B - A).cross(C - A);

    float area2 = N.length_squared();
    if (area2 == 0.f) return FLT_MAX; //incase triangle is wack

    N.normalize();  

    //projecting point onto the triangle
    Vector3 P_proj = P - (P - A).dot(N) * N;    

    //barycentric test
    Vector3 v0 = B - A;
    Vector3 v1 = C - A;
    Vector3 v2 = P_proj - A;    

    float d00 = v0.dot(v0);
    float d01 = v0.dot(v1);
    float d11 = v1.dot(v1);
    float d20 = v2.dot(v0);
    float d21 = v2.dot(v1);

    float denom = d00 * d11 - d01 * d01;
    if (fabs(denom) < 1e-8f) return FLT_MAX;

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1 - v - w;    
    Vector3 candidate;  
    if (v >= 0 && w >= 0 && u >= 0)  {
        candidate = P_proj;
    } else {
        auto closestOnSegment = [](Vector3 _P, Vector3 _A, Vector3 _B) {
            Vector3 AB = _B - _A;
            float denom = AB.dot(AB);                
            if (denom == 0.f)
                return _A;  
            float t = (_P - _A).dot(AB) / denom;
            t = Math::clamp(t, 0.f, 1.f);
            return _A + t * AB;
        };  

        Vector3 pAB = closestOnSegment(P, A, B);
        Vector3 pBC = closestOnSegment(P, B, C);
        Vector3 pCA = closestOnSegment(P, C, A);    

        float dAB = (P - pAB).length_squared();
        float dBC = (P - pBC).length_squared();
        float dCA = (P - pCA).length_squared(); 

        if (dAB < dBC && dAB < dCA) candidate = pAB;
        else if (dBC < dCA)         candidate = pBC;
        else                        candidate = pCA;
    }

    return (P - candidate).length();
}

auto SDFOctree::distance_to_aabb(Vector3 _point, AABB _bounds) -> float {
    Vector3 min = _bounds.position;
    Vector3 max = _bounds.position + _bounds.size;

    float dx = std::max({min.x - _point.x, 0.f, _point.x - max.x});
    float dy = std::max({min.y - _point.y, 0.f, _point.y - max.y});
    float dz = std::max({min.z - _point.z, 0.f, _point.z - max.z});

    return sqrt(dx*dx + dy*dy + dz*dz);
}


auto SDFOctree::sdf_query(Vector3 _point, OctreeNode* _node, float& _best) -> float {
    float box_dist = distance_to_aabb(_point, _node->bounds);

    if (box_dist > _best)
        return _best;

    if (_node->is_leaf()) {
        auto& items = registry.GetChunk(_node->chunk_id)->Items();

        for (auto& [tri, _] : items) {
            float dist = get_tri_point_dist(_point, tri);
            
            if (dist < _best)
                _best = dist;
        }
    } else {
        for (int i = 0; i < 8; ++i)
            sdf_query(_point, _node->children[i].get(), _best);
    }

    return _best;
}


void SDFOctree::draw_debug(Color _color) {
    if (!root_node) return;

    draw_debug_recursive(root_node.get(), _color);

}

void SDFOctree::draw_debug_recursive(OctreeNode* _node, Color _color) {
    if (!_node) return;

    DebugDrawer::get_singleton()->draw_aabb(_node->bounds, _color);

    if (!_node->children[0]) return;

    for (int i = 0; i < 8; ++i)
        draw_debug_recursive(_node->children[i].get(), _color);
}
*/