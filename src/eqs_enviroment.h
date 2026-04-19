#pragma once

#include "godot_cpp/classes/node3D.hpp"
#include "godot_cpp/classes/random_number_generator.hpp"

#include "sdf_evaluator.h"
#include "spatial_octree.h"
#include "eqs_navigation_mesh.h"

using namespace godot;


class EQSEnviroment : public Node3D {
	GDCLASS(EQSEnviroment, Node3D)

public:
	struct InfluencePoint {
		InfluencePoint(Vector3 _position, float _weight)
			: position(_position)
			, test_weight(_weight)
		{}
		Vector3 position;
		float test_weight;
	};



private:
	class InfluenceOctree : public SpatialOctree<InfluencePoint> {
	public:
		InfluenceOctree(int _element_limit, int _depth_limit, AABB _bounds)
			: SpatialOctree<InfluencePoint>(_element_limit, _depth_limit, _bounds)
		{}
		auto get_position(const InfluencePoint& _data) -> Vector3 override { return _data.position; }
	};

	std::unique_ptr<InfluenceOctree> octree;
	EQSNavigationMesh nav_mesh;

	MeshInstance3D* mesh = nullptr;

	int split_limit = 5;
	int depth_limit = 5;

public:
	void _ready() override;

	void draw_debug(Color _color);
	
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("draw_debug", "_color"), &EQSEnviroment::draw_debug);
		
		ClassDB::bind_method(D_METHOD("set_mesh", "_mesh"), &EQSEnviroment::set_mesh);
		ClassDB::bind_method(D_METHOD("get_mesh"), &EQSEnviroment::get_mesh);
		ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "mesh", PROPERTY_HINT_NODE_TYPE, "MeshInstance3D"), "set_mesh", "get_mesh");
	}

	void set_mesh(MeshInstance3D* _mesh) { mesh = _mesh; }
	auto get_mesh() -> MeshInstance3D* { return mesh; }
};
