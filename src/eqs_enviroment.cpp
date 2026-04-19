#include "eqs_enviroment.h"

void EQSEnviroment::_ready() {
	if (!mesh) return;

	octree = std::make_unique<InfluenceOctree>(
		split_limit,
		depth_limit,
		mesh->get_aabb()
	);

	octree->init();

	nav_mesh.generate(mesh);
}


void EQSEnviroment::draw_debug(Color _color) {
	if (octree)
		octree->draw_debug(_color);

	nav_mesh.draw_debug();
}