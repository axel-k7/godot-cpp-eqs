#include "sdf_navmesh.h"


void SDFNavigationMesh::generate(Node* _root) {    
    generator.generate(_root);

    TypedArray<Vector3> points = generator.get_points();
    const std::vector<Triangle> triangles = generator.get_tris();
    //above is interchangable with any way to get the relevant list of triangles & points
    //should actually get mesh triangles in here instead and put them in a temporary octree
    //to cut down on baking time in the loop below

    print_line("point count: ", points.size());
    print_line("triangle count: ", triangles.size());

    graph.clear();
    graph.reserve(points.size());

    for (int i = 0; i < points.size(); ++i) {
        //checking with every triangle totally isn't expensive ahaha
        create_point(points[i], triangles);
    }

    map_neighbours(triangles);

    print_line("graph generated, size: ", graph.size());
}

void SDFNavigationMesh::create_point(Vector3 _position, const std::vector<Triangle>& _tris) {
    //maybe make increment be based on size of relevant geometry
    Vector3 sample_point = Vector3(_position) + Vector3(0, 0.1f, 0);
    Vector3 gradient = SDFEvaluator::gradient(sample_point, _tris);

    float slope = 1.f - gradient.dot(Vector3(0,1,0));
    if (slope > max_slope) return;

    graph.push_back( WalkablePoint{_position, slope} );
}

void SDFNavigationMesh::map_neighbours(const std::vector<Triangle>& _tris){
    float max_dist = max_neighbour_dist*max_neighbour_dist;

    for (size_t i = 0; i < graph.size(); i++) {
        for (size_t j = i+1; j < graph.size(); j++) {
            auto& a = graph[i];
            auto& b = graph[j];

            if ((a.position - b.position).length_squared() < max_dist) {
                Vector3 midpoint = (a.position + b.position) * 0.5f;

                if (SDFEvaluator::sdf(midpoint, _tris) > min_clearance) {
                    a.neighbours.push_back(j);
                    b.neighbours.push_back(i);
                }
            }
        }
    }
}




void SDFNavigationMesh::draw_debug() {
    DebugDrawer* debug = DebugDrawer::get_singleton();
    for (const auto& point : graph) {
        debug->draw_sphere(point.position, 0.5f, Color(point.exposure, point.exposure, point.exposure));
        
        for (size_t idx : point.neighbours) {
            debug->draw_line(point.position, graph[idx].position, Color(0,1,0));
        }
    }
    
    //generator.draw_debug();
}