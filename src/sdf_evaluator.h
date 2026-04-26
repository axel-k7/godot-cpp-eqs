#pragma once

#include <functional>

#include "godot_cpp/variant/aabb.hpp"

using namespace godot;

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


class SDFEvaluator {
public:
	static auto point_to_tri(Vector3 _point, Triangle tri) -> float;
	static auto point_to_aabb(Vector3 _point, AABB _bounds) -> float;
	static auto sdf(Vector3 _point, const std::vector<Triangle>& _tris) -> float;
	static auto gradient(Vector3 _point, const std::vector<Triangle>& _tris, float _epsilon = 0.01f) -> Vector3;




	//auto sdf_query(Vector3 _point, OctreeNode* _node, float& _best)  -> float;

};
