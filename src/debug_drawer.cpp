#include "debug_drawer.h"

DebugDrawer* DebugDrawer::singleton = nullptr;
const int DebugDrawer::aabb_indices[24] = {
		0, 1, 1, 2, 2, 3, 3, 0, // Bottom
        4, 5, 5, 6, 6, 7, 7, 4, // Top
        0, 4, 1, 5, 2, 6, 3, 7  // Pillars
};

void DebugDrawer::_ready() {
    singleton = this;
    init();
    print_line("initialized debug drawer");
}

void DebugDrawer::_process(double delta) {
    draw();
}


void DebugDrawer::init() {
    mesh_instance = memnew(MeshInstance3D);
	add_child(mesh_instance);

	mesh_instance->set_as_top_level(true);
	mesh_instance->set_global_transform(Transform3D());

	mesh.instantiate();
	material.instantiate();

	material->set_shading_mode(StandardMaterial3D::SHADING_MODE_UNSHADED);
    
    material->set_flag(StandardMaterial3D::FLAG_ALBEDO_FROM_VERTEX_COLOR, true);

	mesh_instance->set_mesh(mesh);
	mesh_instance->set_material_override(material);
}


void DebugDrawer::draw() {
    mesh->clear_surfaces();

    if (lines.empty()) return;

    mesh->surface_begin(Mesh::PRIMITIVE_LINES);

    for (const Line& line : lines) {
        mesh->surface_set_color(line.color);
        mesh->surface_add_vertex(line.from);
        mesh->surface_add_vertex(line.to);
    }
    mesh->surface_end();

    lines.clear();
}


void DebugDrawer::draw_line(const Vector3 _from, const Vector3 _to, Color _color) {
    lines.push_back({_from, _to, _color});
}


void DebugDrawer::draw_aabb(AABB _aabb, Color _color) {
    Vector3 min = _aabb.position;
    Vector3 max = _aabb.position + _aabb.size;

    Vector3 vertices[8] = {
        Vector3(min.x, min.y, min.z), Vector3(max.x, min.y, min.z),
        Vector3(max.x, max.y, min.z), Vector3(min.x, max.y, min.z),
        Vector3(min.x, min.y, max.z), Vector3(max.x, min.y, max.z),
        Vector3(max.x, max.y, max.z), Vector3(min.x, max.y, max.z)
    };

    for (int i = 0; i < 24; i += 2) 
        draw_line(vertices[aabb_indices[i]], vertices[aabb_indices[i+1]], _color);
}