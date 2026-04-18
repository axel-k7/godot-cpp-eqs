#pragma once

#include "godot_cpp/variant/typed_array.hpp"

#include "debug_drawer.h"

using namespace godot;


struct Vector3Hash {
	size_t operator()(const Vector3& _vector) const {
		return std::hash<float>{}(_vector.x) 
			^ ( std::hash<float>{}(_vector.y) << 1 )
			^ ( std::hash<float>{}(_vector.z) << 2 );
	}
};

class EQSNavigationMesh : public Node3D {
	GDCLASS(EQSNavigationMesh, Node3D)

private:
	struct Triangle {
		Triangle(const Vector3 _e1, const Vector3 _e2, const Vector3 _e3) {
			edges[0] = _e1;
			edges[1] = _e2;
			edges[2] = _e3;
		}

		Vector3 edges[3];
	};

	struct NavigationNode {
		Triangle tri;
		Vector3 point;
		
		size_t neighbours[3];
		int n_count = 0;

		void PushNeighbour(size_t _index) {
			neighbours[n_count++] = _index;
		}
	};

	
	std::vector<NavigationNode> nodes;
	std::unordered_map<Vector3, std::pair<size_t, size_t>, Vector3Hash> edge_map; 
	
public:
	void generate(MeshInstance3D* _mesh);
	void draw_debug();
	auto get_points() -> TypedArray<Vector3>;

	
//godot boilerplate
public:
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("generate", "_mesh"), &EQSNavigationMesh::generate);
		ClassDB::bind_method(D_METHOD("get_points"), &EQSNavigationMesh::get_points);
		ClassDB::bind_method(D_METHOD("draw_debug"), &EQSNavigationMesh::draw_debug);
	}

};
