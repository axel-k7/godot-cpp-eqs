#pragma once


#include "godot_cpp/variant/aabb.hpp"

#include "debug_drawer.h"
#include "registry.h"

using namespace godot;
/*
struct Triangle {
	Triangle() {
		vertices[0] = Vector3();
		vertices[1] = Vector3();
		vertices[2] = Vector3();
		aabb = AABB();
	}

	Triangle(const Vector3 _e1, const Vector3 _e2, const Vector3 _e3) {
		vertices[0] = _e1;
		vertices[1] = _e2;
		vertices[2] = _e3;

		Vector3 min = _e1.min(_e2).min(_e3);
		Vector3 max = _e1.max(_e2).max(_e3);

		aabb = AABB(min, max - min);
	}

	Vector3 vertices[3];
	AABB aabb;
};


class SDFOctree : public Node3D {
	GDCLASS(SDFOctree, Node3D)

private:
	struct OctreeNode {
		OctreeNode(size_t _chunk_id, AABB _bounds, int _depth)
			: chunk_id(_chunk_id)
			, bounds(_bounds)
    		, depth(_depth)
		{}

		std::unique_ptr<OctreeNode> children[8];
		AABB bounds;

		float corners[8];

		size_t chunk_id;
		int depth;

		bool is_leaf() const { return children[0] == nullptr; }
	};

	Registry<Triangle> registry;

	std::vector<OctreeNode*> pending_splits;

	std::unique_ptr<OctreeNode> root_node;
	int depth_limit = 5;
	int element_limit = 5;

	float base_size = 10;


	void split(OctreeNode* _node);
	void merge(OctreeNode* _node);

	auto try_populate_node(OctreeNode* _node, Triangle _tri) -> bool;
	
	auto find_node(Vector3 _point) -> OctreeNode*;
	auto find_node_recursive(Vector3 _point, OctreeNode* _node) -> OctreeNode*;
	
	void draw_debug_recursive(OctreeNode* _node, Color _color);

	auto get_tri_point_dist(Vector3 _point, Triangle tri) -> float;
	auto distance_to_aabb(Vector3 _point, AABB _bounds) -> float;

	auto sdf_query(Vector3 _point, OctreeNode* _node, float& _best)  -> float;

	
public:
	auto try_insert(Triangle _tri) -> bool;
	auto try_insert_v(Vector3 _a, Vector3 _b, Vector3 _c) -> bool { return try_insert(Triangle{_a, _b, _c}); }
	void draw_debug(Color _color);

	auto get_tris(Vector3 _point) { return registry.GetChunk(find_node(_point)->chunk_id)->Items(); }
	auto get_sdf(Vector3 _point) -> float;
	auto get_sdf_fast(Vector3 _point) -> float { float best = FLT_MAX; return sdf_query(_point, root_node.get(), best); };
	auto brute_force_sdf(Vector3 _point) -> float;

	void bake(OctreeNode* _node);
	void root_bake() { bake(root_node.get()); }
	
	void _ready() override;

	void init();
	




//godot boilerplate
public:
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("try_insert", "_a", "_b", "_c"), &SDFOctree::try_insert_v);
		ClassDB::bind_method(D_METHOD("get_sdf", "_point"), &SDFOctree::get_sdf);
		ClassDB::bind_method(D_METHOD("draw_debug", "_color"), &SDFOctree::draw_debug);
		ClassDB::bind_method(D_METHOD("init"), &SDFOctree::init);
		ClassDB::bind_method(D_METHOD("bake"), &SDFOctree::root_bake);
		
		ClassDB::bind_method(D_METHOD("get_base_size"), &SDFOctree::get_base_size);
		ClassDB::bind_method(D_METHOD("set_base_size", "_value"), &SDFOctree::set_base_size);
		ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "base_size", PROPERTY_HINT_RANGE, "1,50"), "set_base_size", "get_base_size");

		ClassDB::bind_method(D_METHOD("get_depth_limit"), &SDFOctree::get_depth_limit);
		ClassDB::bind_method(D_METHOD("set_depth_limit", "_value"), &SDFOctree::set_depth_limit);
		ADD_PROPERTY(PropertyInfo(Variant::INT, "depth_limit", PROPERTY_HINT_RANGE, "1,50"), "set_depth_limit", "get_depth_limit");

		ClassDB::bind_method(D_METHOD("get_element_limit"), &SDFOctree::get_element_limit);
		ClassDB::bind_method(D_METHOD("set_element_limit", "_value"), &SDFOctree::set_element_limit);
		ADD_PROPERTY(PropertyInfo(Variant::INT, "element_limit", PROPERTY_HINT_RANGE, "1,50"), "set_element_limit", "get_element_limit");
	}


	float get_base_size() const { return base_size; };
	void set_base_size(float _value) { base_size = _value; };

	int get_depth_limit() const { return depth_limit; };
	void set_depth_limit(int _value) { depth_limit = _value; };

	int get_element_limit() const { return element_limit; };
	void set_element_limit(float _value) { element_limit = _value; };

};

*/