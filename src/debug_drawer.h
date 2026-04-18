#pragma once

#include <deque>

#include "godot_cpp/classes/mesh_instance3d.hpp"
#include "godot_cpp/classes/immediate_mesh.hpp"
#include "godot_cpp/classes/standard_material3d.hpp"

#include "godot_cpp/variant/aabb.hpp"

using namespace godot;


class DebugDrawer : public Node3D {
	GDCLASS(DebugDrawer, Node3D)

private:
	struct Line {
		Vector3 from;
		Vector3 to;
		Color color;
	};

	static DebugDrawer* singleton;
	static const int aabb_indices[24];

	std::vector<Line> lines;

	MeshInstance3D* mesh_instance = nullptr;
	Ref<ImmediateMesh> mesh;
	Ref<StandardMaterial3D> material;

	void init();
	void draw();

public:
	static DebugDrawer* get_singleton() { return singleton; }

	void draw_line(const Vector3 _from, const Vector3 _to, Color _color);
	void draw_aabb(const AABB _aabb, Color _color);

	void _process(double delta) override;
	void _ready() override;

//godot boilerplate
public:
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("draw_line", "_from", "_to", "_color"), &DebugDrawer::draw_line);
		ClassDB::bind_method(D_METHOD("draw_aabb", "_aabb", "_color"), &DebugDrawer::draw_aabb);
	}
};
