#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/variant/aabb.hpp"
#include "godot_cpp/variant/variant.hpp"

using namespace godot;

class Octree : public RefCounted {
	GDCLASS(Octree, RefCounted)

private:
	struct OctreeNode {
		AABB bounds;
	};

protected:
	static void _bind_methods();

public:
	Octree() = default;
	~Octree() override = default;
};
