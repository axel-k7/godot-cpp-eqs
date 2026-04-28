/*#include "eqs_enviroment.h"

void EQSEnviroment::_ready() {
	if (Engine::get_singleton()->is_editor_hint()) return;

	if (!root) return;

	/*disable octree for now, will figure out a way to do aabb with node later
	AABB global_aabb = root->get_global_transform().xform( mesh->get_aabb() );

	global_aabb.grow_by(1.2);

	octree = std::make_unique<InfluenceOctree>(
		split_limit,
		depth_limit,
		global_aabb
	);

	octree->init();


	nav_mesh.generate(root);
}


void EQSEnviroment::draw_debug(Color _color) {
	if (octree)
		octree->draw_debug(_color);

	nav_mesh.draw_debug();
}*/
