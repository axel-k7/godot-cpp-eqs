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
		OctreeNode(AABB _bounds, RID _area_rid, RID _shape_rid, int _depth)
    		: bounds(_bounds)
    		, area_rid(_area_rid)
			, shape_rid(_shape_rid)
    		, depth(_depth)
		{}

		~OctreeNode() {
			PhysicsServer3D* phys3 = PhysicsServer3D::get_singleton();

			if (area_rid.is_valid())
				phys3->free_rid(area_rid);

			if (shape_rid.is_valid())
				phys3->free_rid(shape_rid);
		}

		AABB bounds;
		RID area_rid;
		RID shape_rid;
		int depth;

		std::unique_ptr<OctreeNode> children[8];
		TypedArray<Node3D> elements;
	};
	
	std::unordered_map<RID, OctreeNode*, RIDHasher> area_map;
	std::unique_ptr<OctreeNode> root_node;
	int depth_limit = 5;
	int element_limit = 3;

	float base_size = 10;

protected:
	static void _bind_methods();

	void split(OctreeNode* _node);
	void merge(OctreeNode* _node);
	//need some way to actually trigger the merge, parent node currently stops caring after split

	
	void validate();
	
	public:
	Octree() = default;
	
	void _ready() override;
	
	void insert(int _status, RID _body_rid, int64_t _instance_id, int _body_shape_index, int _area_shape_index, RID _area_rid);

	float get_base_size() const { return base_size; };
	void set_base_size(float _value) { base_size = _value; };










//temp debug drawing
private:
	MeshInstance3D* debug_mesh_instance = nullptr;
	Ref<ImmediateMesh> debug_mesh;
	Ref<StandardMaterial3D> debug_material;

	void Octree::init_debug_draw() {
		debug_mesh_instance = memnew(MeshInstance3D);
		add_child(debug_mesh_instance);

		debug_mesh_instance->set_as_top_level(true);
		debug_mesh_instance->set_global_transform(Transform3D());

		debug_mesh.instantiate();
		debug_material.instantiate();

		debug_material->set_shading_mode(StandardMaterial3D::SHADING_MODE_UNSHADED);
		debug_material->set_albedo(Color(1,0,0));

		debug_mesh_instance->set_mesh(debug_mesh);
		debug_mesh_instance->set_material_override(debug_material);

		update_debug();
	}

	void Octree::update_debug() {
		if (debug_mesh.is_null() || !root_node) return;

		debug_mesh->clear_surfaces();
		debug_mesh->surface_begin(Mesh::PRIMITIVE_LINES);
		
		draw_debug(root_node.get());
	
		debug_mesh->surface_end();

		debug_mesh_instance->set_custom_aabb(root_node->bounds);
	}

	void Octree::draw_debug(OctreeNode* _node) {
		if (!_node) return;

		add_aabb_debug(_node->bounds);

		if (_node->children[0]) {
			for (int i = 0; i < 8; ++i) {
				draw_debug(_node->children[i].get());
			}
   	 }
	}

	void Octree::add_aabb_debug(const AABB &p_aabb) {
    	Vector3 min = p_aabb.position;
   		Vector3 max = p_aabb.position + p_aabb.size;

		Vector3 c[8] = {
		    Vector3(min.x, min.y, min.z), Vector3(max.x, min.y, min.z),
		    Vector3(max.x, max.y, min.z), Vector3(min.x, max.y, min.z),
		    Vector3(min.x, min.y, max.z), Vector3(max.x, min.y, max.z),
		    Vector3(max.x, max.y, max.z), Vector3(min.x, max.y, max.z)
		};

		int indices[24] = {
		    0, 1, 1, 2, 2, 3, 3, 0, // Bottom square
		    4, 5, 5, 6, 6, 7, 7, 4, // Top square
		    0, 4, 1, 5, 2, 6, 3, 7  // Vertical pillars
		};

		for (int i = 0; i < 24; ++i) {
		    debug_mesh->surface_add_vertex(c[indices[i]]);
		}
	}

};