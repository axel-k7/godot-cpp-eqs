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

		std::vector<size_t> neighbours; //index in graph
	};

	std::vector<WalkablePoint> graph;

	float max_slope = 1.f;

	float max_neighbour_dist = 5.f;
	float min_clearance = 0.1f;

	void generate(Node* _root);
	void draw_debug();
private:
	NavPointGenerator generator;

	void create_point(Vector3 _position, const std::vector<Triangle>& _tris);
	void map_neighbours(const std::vector<Triangle>& _tris);
};
