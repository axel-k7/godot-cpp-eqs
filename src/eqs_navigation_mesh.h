#pragma once

#include "godot_cpp/variant/typed_array.hpp"

#include "debug_drawer.h"
#include "sdf_evaluator.h"

using namespace godot;


struct Vector3Hash {
	size_t operator()(const Vector3& _vector) const {
		return  std::hash<float>{}(_vector.x) 
			^ ( std::hash<float>{}(_vector.y) << 1 )
			^ ( std::hash<float>{}(_vector.z) << 2 );
	}
};

//very simple nav-mesh generator
//basically just copies the original mesh triangles and maps their neighbours
class EQSNavigationMesh {
private:
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
	auto get_tris() -> std::vector<Triangle>;
};
