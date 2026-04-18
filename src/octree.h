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

using namespace godot;


struct RIDHasher {
	std::size_t operator()(const RID& _rid) const {
		return std::hash<uint64_t>()(_rid.get_id());
	}
};


//maybe make a general data-partitioner class? might make inheritance too annoying
class Octree : public Node3D {
	GDCLASS(Octree, Node3D)

private:
	struct OctreeNode {
		OctreeNode(OctreeNode* _parent, AABB _bounds, RID _area_rid, RID _shape_rid)
    		: parent(_parent)
			, bounds(_bounds)
    		, area_rid(_area_rid)
			, shape_rid(_shape_rid)
    		, depth(parent ? parent->depth+1 : 0)
		{}

		~OctreeNode() {
			PhysicsServer3D* phys3 = PhysicsServer3D::get_singleton();

			if (area_rid.is_valid())
				phys3->free_rid(area_rid);

			if (shape_rid.is_valid())
				phys3->free_rid(shape_rid);
		}
		
		std::unique_ptr<OctreeNode> children[8];
		OctreeNode* parent;
		AABB bounds;
		RID area_rid;
		RID shape_rid;
		int depth;

		int count;
	};

	std::unordered_map<RID, OctreeNode*, RIDHasher> area_map;
	std::unordered_map<int64_t, int> overlap_count;
	std::unique_ptr<OctreeNode> root_node;
	int depth_limit = 5;
	int element_limit = 3;

	float base_size = 10;

	
	void split(OctreeNode* _node);
	void merge(OctreeNode* _node);

	void draw_debug_recursive(OctreeNode* _node);

public:
	void insert(int _status, RID _body_rid, int64_t _instance_id, int _body_shape_index, int _area_shape_index, RID _area_rid);
	void draw_debug();
	
	void _ready() override;
	




//godot boilerplate
public:
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("insert", "_status", "_body_rid", "_instance_id", "_body_shape_index", "_area_shape_index", "_area_rid"), &Octree::insert);
		ClassDB::bind_method(D_METHOD("draw_debug"), &Octree::draw_debug);
		
		ClassDB::bind_method(D_METHOD("get_base_size"), &Octree::get_base_size);
		ClassDB::bind_method(D_METHOD("set_base_size", "value"), &Octree::set_base_size);
		ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "base_size", PROPERTY_HINT_RANGE, "1,50"), "set_base_size", "get_base_size");
	}


	float get_base_size() const { return base_size; };
	void set_base_size(float _value) { base_size = _value; };

};