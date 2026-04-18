extends Octree

@export var insert: bool = false
@export var nav_mesh: EQSNavigationMesh

func _ready():
	init()
#	var min_range := -3
#	var max_range := 3
#	
#	for i in range(15):
#		var random_vec = Vector3(
#			randf_range(min_range, max_range),
#			randf_range(min_range, max_range),
#			randf_range(min_range, max_range)
#		)
#		try_insert(random_vec)

func _process(delta):
	if insert:
		for point in nav_mesh.get_points():
			try_insert(point)
		insert = false
		

	draw_debug(Color(0,0,1))
