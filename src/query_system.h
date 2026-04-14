#pragma once

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/variant/variant.hpp"

using namespace godot;

class QuerySystem : public Node {
	GDCLASS(QuerySystem, Node)

private:
	float test_float;

protected:
	static void _bind_methods();

public:
	QuerySystem();
	~QuerySystem() override = default;

	void _process(double _delta) override;

	void set_test_float(const double _test_float);
	double get_test_float() const;
};
