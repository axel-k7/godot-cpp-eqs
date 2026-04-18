extends EQSNavigationMesh

@onready var mesh: MeshInstance3D = $mesh

func _ready():
	if mesh:
		generate(mesh)


func _process(delta):
	draw_debug()
