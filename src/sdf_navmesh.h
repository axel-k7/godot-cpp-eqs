#pragma once

#include "godot_cpp/variant/typed_array.hpp"

#include "debug_drawer.h"
#include "nav_point_generator.h"
#include "sdf_evaluator.h"

using namespace godot;


class SDFNavigationMesh {
public:
	struct WalkablePoint {
		WalkablePoint(Vector3 _position, float _exposure)
			: position(_position)
			, exposure(_exposure)
		{}
		Vector3 position;
		float exposure;
	};

	float max_slope = 1.f;
	std::vector<WalkablePoint> graph;

	void generate(MeshInstance3D* _mesh);
	void draw_debug();
private:
	NavPointGenerator generator;
};
