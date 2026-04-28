#pragma once

#include <functional>

#include "godot_cpp/variant/aabb.hpp"
#include "structure_definitions.h"

using namespace godot;

class SDFEvaluator {
public:
	static auto point_to_tri(Vector3 _point, Triangle tri) -> float;
	static auto point_to_aabb(Vector3 _point, AABB _bounds) -> float;
	static auto sdf(Vector3 _point, const std::vector<Triangle>& _tris) -> float;
	static auto gradient(Vector3 _point, const std::vector<Triangle>& _tris, float _epsilon = 0.01f) -> Vector3;




	//auto sdf_query(Vector3 _point, OctreeNode* _node, float& _best)  -> float;

};
