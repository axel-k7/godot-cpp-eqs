extends SDFOctree

@export var do_insert: bool = false
@export var do_bake: bool = false
@export var do_random_sdf: bool = false
@export var nav_mesh: SDFNavigationMesh

func _ready():
	init()

func _process(delta):
	if do_insert:
		var tris = nav_mesh.get_tris();
		for i in range(0, tris.size(), 3):
			try_insert(tris[i], tris[i+1], tris[i+2])
		do_insert = false
	
	if do_bake:
		bake()
		do_bake = false
		
	if (do_random_sdf):
		var min := -base_size/2
		var max := base_size/2
		var random_point = Vector3(
			randi_range(min, max),
			randi_range(min, max),
			randi_range(min, max)
		)
		print(get_sdf(random_point))
		do_random_sdf = false

	draw_debug(Color(0,0,1))
