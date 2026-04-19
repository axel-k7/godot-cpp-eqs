#include "sdf_navmesh.h"


void SDFNavigationMesh::generate(MeshInstance3D* _mesh) {    
    generator.generate(_mesh);

    TypedArray<Vector3> points = generator.get_points();
    const std::vector<Triangle> triangles = generator.get_tris();

    print_line("point count: ", points.size());
    print_line("triangle count: ", triangles.size());

    graph.clear();
    graph.reserve(points.size());

    for (int i = 0; i < points.size(); ++i) {

        //checking with every triangle totally isn't expensive ahaha
        Vector3 sample_point = Vector3(points[i]) + Vector3(0, 0.1f, 0);
        Vector3 gradient = SDFEvaluator::gradient(sample_point, triangles);

        float slope = 1.f - gradient.dot(Vector3(0,1,0));
        if (slope > max_slope) continue;


        graph.push_back( WalkablePoint{points[i], slope} );
    }

    print_line("graph generated, size: ", graph.size());
}

void SDFNavigationMesh::draw_debug() {
    DebugDrawer* debug = DebugDrawer::get_singleton();
    for (const auto& point : graph) 
        debug->draw_sphere(point.position, 0.01f, Color(point.exposure, point.exposure, point.exposure));
    
    //generator.draw_debug();
}