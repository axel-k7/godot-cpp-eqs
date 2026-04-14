#include "query_system.h"

QuerySystem::QuerySystem() {
	test_float = 10.0;
}

void QuerySystem::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_test_float"), &QuerySystem::get_test_float);
	ClassDB::bind_method(D_METHOD("set_test_float", "_test_float"), &QuerySystem::set_test_float);


	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "test_float"), "set_test_float", "get_test_float");
}

void QuerySystem::_process(double delta) {
	
}

void QuerySystem::set_test_float(const double _test_float) {
	test_float = _test_float;
}

double QuerySystem::get_test_float() const {
	return test_float;
}
