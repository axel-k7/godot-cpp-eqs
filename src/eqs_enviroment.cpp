#include "eqs_enviroment.h"

void EQSEnviroment::_ready() {
	if (!mesh) return;

	AABB global_aabb = mesh->get_global_transform().xform( mesh->get_aabb() );

	octree = std::make_unique<InfluenceOctree>(
		split_limit,
		depth_limit,
		global_aabb
	);

	octree->init();

	nav_mesh.generate(mesh);
	
	auto points = nav_mesh.get_points();
	for (int i = 0; i < points.size(); i++) {
		octree->try_insert(
			InfluencePoint{points[i], float(UtilityFunctions::randf())}
		);
	}

}


void EQSEnviroment::draw_debug(Color _color) {
	if (octree)
		octree->draw_debug(_color);

	nav_mesh.draw_debug();
}