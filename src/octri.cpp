#include "octri.h"

void Octri::build_tree(const std::vector<Triangle>& _triangles) {
	triangles = _triangles; //maybe shouldn't copy?

	for (int i = 0; i < triangles.size(); i++)
		try_insert(root_node.get(), i);
}


void Octri::try_insert(OctriNode* _node, int _index) {
	if (!_node->bounds.intersects(triangles[_index].aabb)) return;

	if (!_node->is_leaf()) {
		for (int i = 0; i < 8; i++)
			try_insert(_node->children[i].get(), _index);

		return;
	}

	_node->tri_ids.push_back(_index);

	if (_node->tri_ids.size() > triangle_limit && _node->depth < depth_limit)
		split(_node);
}


void Octri::split(OctriNode* _node) {
	Vector3 half_size = _node->bounds.size * 0.5f;

	for (int i = 0; i < 8; i++) {
		Vector3 offset(
			(i & 1) ? half_size.x : 0.f,
			(i & 2) ? half_size.y : 0.f,
			(i & 4) ? half_size.z : 0.f
		);
		//(000) = left, bottom, front
		//(111) = right, top, back

		AABB child_bounds(_node->bounds.position + offset, half_size);

		_node->children[i] = std::make_unique<OctriNode>(child_bounds, _node->depth+1);

		for (int index : _node->tri_ids)
			try_insert(_node->children[i].get(), index);
	}

	_node->tri_ids.clear();
	_node->tri_ids.shrink_to_fit();
}

auto Octri::get_node(const Vector3& _point) const -> OctriNode* {
	if (!root_node->bounds.has_point(_point)) return nullptr;

	OctriNode* node = root_node.get();
	while (!node->is_leaf()) {
		Vector3 center = node->bounds.position + node->bounds.size*0.5;

		int index = 0;
		if (_point.x > center.x) index |= 1;
		if (_point.y > center.y) index |= 2;
		if (_point.z > center.z) index |= 4;

		node = node->children[index].get();
	}

	return node;
}

void Octri::get_closet_dist_recursive(const OctriNode* _node, const Vector3& _point, float& _min_dist) const {
	float aabb_dist = SDFEvaluator::point_to_aabb(_point, _node->bounds);
	if (aabb_dist >= _min_dist) return;

	if (!_node->is_leaf()) {
		for (int i = 0; i < 8; i++)
			get_closet_dist_recursive(_node->children[i].get(), _point, _min_dist);
			//can calculate closest child and search that one first

		return;
	}

	for (int index : _node->tri_ids) {
		float dist = SDFEvaluator::point_to_tri(_point, triangles[index]);

		if (dist < _min_dist)
			_min_dist = dist;
	}
}

auto Octri::get_closest_distance(const Vector3& _point) const -> float {
	float min_dist = FLT_MAX;

	get_closet_dist_recursive(root_node.get(), _point, min_dist);

	return min_dist;
}
