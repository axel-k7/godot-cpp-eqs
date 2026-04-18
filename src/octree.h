#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/area3d.hpp"
#include "godot_cpp/classes/world3d.hpp"
#include "godot_cpp/classes/physics_server3d.hpp"
#include "godot_cpp/classes/immediate_mesh.hpp"
#include "godot_cpp/classes/mesh_instance3d.hpp"
#include "godot_cpp/classes/standard_material3d.hpp"

#include "godot_cpp/variant/aabb.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/typed_array.hpp"

#include "debug_drawer.h"
#include "registry.h"

using namespace godot;

struct InfluencePoint {
	Vector3 position;
	//float some_weight;
};

class Octree : public Node3D {
	GDCLASS(Octree, Node3D)

private:
	struct OctreeNode {
		OctreeNode(size_t _chunk_id, AABB _bounds, int _depth)
			: chunk_id(_chunk_id)
			, bounds(_bounds)
    		, depth(_depth)
		{}

		std::unique_ptr<OctreeNode> children[8];
		AABB bounds;
		size_t chunk_id;
		int depth;
	};

	std::unique_ptr<OctreeNode> root_node;
	int depth_limit = 5;
	int element_limit = 3;

	float base_size = 10;

	Registry<InfluencePoint> registry;

	
	void split(OctreeNode* _node);
	void merge(OctreeNode* _node);

	auto find_node(Vector3 _point) -> OctreeNode*;
	auto find_node_recursive(OctreeNode* _node, Vector3 _point) -> OctreeNode*;

	void draw_debug_recursive(OctreeNode* _node);

public:
	auto try_insert(Vector3 _point) -> size_t;
	void draw_debug();
	
	void _ready() override;
	




//godot boilerplate
public:
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("try_insert", "_point"), &Octree::try_insert);
		ClassDB::bind_method(D_METHOD("draw_debug"), &Octree::draw_debug);
		
		ClassDB::bind_method(D_METHOD("get_base_size"), &Octree::get_base_size);
		ClassDB::bind_method(D_METHOD("set_base_size", "value"), &Octree::set_base_size);
		ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "base_size", PROPERTY_HINT_RANGE, "1,50"), "set_base_size", "get_base_size");
	}


	float get_base_size() const { return base_size; };
	void set_base_size(float _value) { base_size = _value; };

};