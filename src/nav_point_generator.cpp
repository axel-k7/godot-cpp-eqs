#include "nav_point_generator.h"


void NavPointGenerator::generate(Node* _root) {
    if (!_root)
        return;
        
    nodes.clear();

    std::vector<MeshInstance3D*> meshes;
    get_meshes_recursive(_root, meshes);

    for (const auto* mesh_instance : meshes) {
        Transform3D world_space = mesh_instance->get_global_transform();
        Ref<Mesh> mesh = mesh_instance->get_mesh();
        

        for (int s = 0; s < mesh->get_surface_count(); s++) {
            Array mesh_arrays = mesh->surface_get_arrays(s);
            PackedVector3Array vertices = mesh_arrays[Mesh::ARRAY_VERTEX];
            PackedInt32Array indices = mesh_arrays[Mesh::ARRAY_INDEX];
            
            for (int i = 0; i < indices.size(); i += 3) {
                const Vector3 p1 = world_space.xform( vertices[indices[i  ]] );
                const Vector3 p2 = world_space.xform( vertices[indices[i+1]] );
                const Vector3 p3 = world_space.xform( vertices[indices[i+2]] );
                
                nodes.push_back({
                    Triangle(p1, p2, p3),
                    (p1 + p2 + p3) / 3
                });

                const Vector3 edge_points[3] = {
                    (p1 + p2) * 0.5f, 
                    (p2 + p3) * 0.5f, 
                    (p3 + p1) * 0.5f,
                };

                for (const auto& edge_point : edge_points) {
                    auto [it, _] = edge_map.try_emplace(edge_point, SIZE_MAX, SIZE_MAX);

                    if (it->second.first == SIZE_MAX)
                        it->second.first = i/3;

                    else 
                        it->second.second = i/3;    
                }
            }
        }
        
        for (const auto& entry : edge_map) {
            if (entry.second.first != SIZE_MAX && entry.second.second != SIZE_MAX) {
                nodes[entry.second.first].PushNeighbour(entry.second.second);
                nodes[entry.second.second].PushNeighbour(entry.second.first);
            }
        }

        edge_map.clear(); //don't need to keep the edge map
    }

    print_line("ran generator, node count: ", nodes.size());
}

auto NavPointGenerator::get_points() -> TypedArray<Vector3> {
    TypedArray<Vector3> points;
    size_t node_amount = nodes.size();
    points.resize(nodes.size());

    for (size_t i = 0; i < node_amount; ++i)
        points[i] = nodes[i].point;
    
    return points;
}


auto NavPointGenerator::get_tris() -> std::vector<Triangle> {
    std::vector<Triangle> tris;
    size_t node_amount = nodes.size();
    tris.resize(node_amount);

    for (size_t i = 0; i < node_amount; ++i)
        tris[i] = nodes[i].tri;
    
    return tris;
}


void NavPointGenerator::get_meshes_recursive(Node* _node, std::vector<MeshInstance3D*>& _result_vector) {
    MeshInstance3D* mesh = Object::cast_to<MeshInstance3D>(_node);
    if (mesh)
        _result_vector.push_back(mesh);

    for (int i = 0; i < _node->get_child_count(); i++)
        get_meshes_recursive(_node->get_child(i), _result_vector);
}


void NavPointGenerator::draw_debug() {
    if (nodes.empty()) return;

    DebugDrawer* debug = DebugDrawer::get_singleton();

    for (const auto& node : nodes) {
        const Vector3* v = node.tri.vertices;

        for (int i = 0; i < 3; i++)
            debug->draw_line(v[i], v[(i + 1) % 3], Color(1,0,0));
        
        for (int n = 0; n < node.n_count; n++) {
            size_t neighbor_idx = node.neighbours[n];
            if (neighbor_idx < nodes.size()) {
                debug->draw_line(node.point, nodes[neighbor_idx].point, Color(0,1,0));
            }
        }
    }
}