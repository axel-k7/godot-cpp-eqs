/*#pragma once

#include "godot_cpp/classes/node3d.hpp"

#include "mesh_decomposer.h"

using namespace godot;

//rename this class lol
class NavmeshGenerator : public Node3D {
	GDCLASS(NavmeshGenerator, Node3D)
private:
	std::vector<ConvexHull> areas;
	GeometrySystem geometry;

public:
	NavmeshGenerator() = default;
	void Generate(MeshInstance3D* _mesh);

	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("Generate", "_mesh"), &NavmeshGenerator::Generate);
	}
};
*/