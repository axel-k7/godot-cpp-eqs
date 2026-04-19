#pragma once

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/node3D.hpp"
#include "godot_cpp/classes/random_number_generator.hpp"

#include "sdf_evaluator.h"
#include "spatial_octree.h"
#include "sdf_navmesh.h"

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
	SDFNavigationMesh nav_mesh;

	Node* root = nullptr;

	int split_limit = 5;
	int depth_limit = 5;

public:
	void _ready() override;

	void draw_debug(Color _color);
	
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("draw_debug", "_color"), &EQSEnviroment::draw_debug);
		
		ClassDB::bind_method(D_METHOD("set_root", "_root"), &EQSEnviroment::set_root);
		ClassDB::bind_method(D_METHOD("get_root"), &EQSEnviroment::get_root);
		ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "root", PROPERTY_HINT_NODE_TYPE, "Node"), "set_root", "get_root");

		ClassDB::bind_method(D_METHOD("set_max_slope", "_max_slope"), &EQSEnviroment::set_max_slope);
		ClassDB::bind_method(D_METHOD("get_max_slope"), &EQSEnviroment::get_max_slope);
		ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_slope"), "set_max_slope", "get_max_slope");

		ClassDB::bind_method(D_METHOD("set_split_limit", "_split_limit"), &EQSEnviroment::set_split_limit);
		ClassDB::bind_method(D_METHOD("get_split_limit"), &EQSEnviroment::get_split_limit);
		ADD_PROPERTY(PropertyInfo(Variant::INT, "split_limit"), "set_split_limit", "get_split_limit");

		ClassDB::bind_method(D_METHOD("set_depth_limit", "_depth_limit"), &EQSEnviroment::set_depth_limit);
		ClassDB::bind_method(D_METHOD("get_depth_limit"), &EQSEnviroment::get_depth_limit);
		ADD_PROPERTY(PropertyInfo(Variant::INT, "depth_limit"), "set_depth_limit", "get_depth_limit");
	}

	auto get_root() -> Node* { return root; }
	void set_root(Node* _root) { root = _root; }
	
	auto get_split_limit() -> int { return split_limit; }
	void set_split_limit(int _split_limit) { split_limit = _split_limit; }
	
	auto get_depth_limit() -> int { return depth_limit; }
	void set_depth_limit(int _depth_limit) { depth_limit = _depth_limit; }

	auto get_max_slope() -> float { return nav_mesh.max_slope; }
	void set_max_slope(float _max_slope) { nav_mesh.max_slope = _max_slope; }
};
