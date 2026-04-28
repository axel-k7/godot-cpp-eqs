#pragma once

#include "godot_cpp/variant/vector3.hpp"

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


//expects worldspace
struct ConvexHull {
	ConvexHull(const std::vector<Triangle>& _triangles) {
		triangles = _triangles;

		aabb = triangles[0].aabb;
		for (const Triangle& tri : triangles)
			aabb.merge_with(tri.aabb);
	}

	std::vector<Triangle> triangles;
	AABB aabb;
};

