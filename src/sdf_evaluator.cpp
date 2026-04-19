#include "sdf_evaluator.h"


auto SDFEvaluator::point_to_tri(Vector3 _point, Triangle _tri) -> float {
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

auto SDFEvaluator::point_to_aabb(Vector3 _point, AABB _bounds) -> float {
    Vector3 min = _bounds.position;
    Vector3 max = _bounds.position + _bounds.size;

    float dx = std::max({min.x - _point.x, 0.f, _point.x - max.x});
    float dy = std::max({min.y - _point.y, 0.f, _point.y - max.y});
    float dz = std::max({min.z - _point.z, 0.f, _point.z - max.z});

    return sqrt(dx*dx + dy*dy + dz*dz);
}

auto SDFEvaluator::sdf(Vector3 _point, const std::vector<Triangle>& _tris) -> float {
    float min_dist = FLT_MAX;

    for (const Triangle& tri : _tris) {
        float dist = point_to_tri(_point, tri);
        if (dist < min_dist)
            min_dist = dist;
    }
    
    return min_dist;
}


auto SDFEvaluator::gradient(Vector3 _point, const std::vector<Triangle>& _tris, float _epsilon) -> Vector3 {
    float x1 = sdf(_point + Vector3(_epsilon, 0, 0), _tris);
    float x2 = sdf(_point - Vector3(_epsilon, 0, 0), _tris);
    float dx = x1 - x2;

    float y1 = sdf(_point + Vector3(0, _epsilon, 0), _tris);
    float y2 = sdf(_point - Vector3(0, _epsilon, 0), _tris);
    float dy = y1 - y2;

    float z1 = sdf(_point + Vector3(0, 0, _epsilon), _tris);
    float z2 = sdf(_point - Vector3(0, 0, _epsilon), _tris);
    float dz = z1 - z2;

    return Vector3(dx, dy, dz).normalized();
}



/*
auto SDFEvaluator::sdf_query(Vector3 _point, OctreeNode* _node, float& _best) -> float {
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
*/