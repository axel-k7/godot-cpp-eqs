#include "octree.h"

void Octree::_ready() {
    PhysicsServer3D* phys3 = PhysicsServer3D::get_singleton();

    AABB root_bounds(get_global_position(), Vector3(base_size, base_size, base_size));

    RID area_rid = phys3->area_create();
    RID box_rid = phys3->box_shape_create();
    
    //box shape wants half extents
    phys3->shape_set_data(box_rid, root_bounds.size * 0.5f);
    phys3->area_add_shape(area_rid, box_rid);

    //setting area position to middle of aabb
    Transform3D transform(Basis(), root_bounds.position + root_bounds.size * 0.5f);
    phys3->area_set_transform(area_rid, transform);

    
    root_node = std::make_unique<OctreeNode>(
        nullptr,
        root_bounds,
        area_rid,
        box_rid
    );
    
    area_map[area_rid] = root_node.get();
    Callable insert_callback = Callable(this, "insert").bind(area_rid);
    
    phys3->area_set_monitor_callback(area_rid, insert_callback);
    phys3->area_set_space(area_rid, get_world_3d()->get_space());

    //TEMP
    print_line("initialized octree with area: ", area_rid);

    init_debug_draw();
}

void Octree::_bind_methods() {
    ClassDB::bind_method(D_METHOD("insert", "_status", "_body_rid", "_instance_id", "_body_shape_index", "_area_shape_index", "_area_rid"), &Octree::insert);

    ClassDB::bind_method(D_METHOD("get_base_size"), &Octree::get_base_size);
    ClassDB::bind_method(D_METHOD("set_base_size", "value"), &Octree::set_base_size);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "base_size", PROPERTY_HINT_RANGE, "1,50"), "set_base_size", "get_base_size");
}

//maybe look to pass area directly somehow to avoid map lookups? callable only accepts variant
void Octree::insert(int _status, RID _body_rid, int64_t _instance_id, int _body_shape_index, int _area_shape_index, RID _area_rid) {
    print_line("insertion ran on area: ", _body_rid);

    auto it = area_map.find(_area_rid);
    if (it == area_map.end()) return;

    print_line("found area: ", _area_rid, " in area map");

    OctreeNode* area = it->second;

    switch (_status) {
    case PhysicsServer3D::AREA_BODY_ADDED:
        print_line("element entered octree node: ", Object::cast_to<Node3D>(ObjectDB::get_instance(ObjectID(_instance_id)))->get_name());
        area->count++;
        overlap_count[_instance_id]++;

        //propogate count if not overlapping multiple nodes
        if (overlap_count[_instance_id] == 1) {
            OctreeNode* ancestor = area->parent;
            while (ancestor) {
                ancestor->count++;
                ancestor = ancestor->parent;
            }
        }

        if(area->count > element_limit)
            split(area);

        break;

    case PhysicsServer3D::AREA_BODY_REMOVED:
        print_line("element exited octree node: ", Object::cast_to<Node3D>(ObjectDB::get_instance(ObjectID(_instance_id)))->get_name());
        area->count--;
        overlap_count[_instance_id]--;

        //propogate count and erase if no longer inside of the tree
        if (overlap_count[_instance_id] == 0) {
            overlap_count.erase(_instance_id);
            OctreeNode* ancestor = area->parent;
            while (ancestor) {
                ancestor->count--;

                if (ancestor->count < element_limit)
                    merge(ancestor);

                ancestor = ancestor->parent;
            }
        }
        //this fails to account for the fact that a node might leave in a different-sub-tree than it entered from

        break;

    default:
        print_line("uhnandled status: ", _status);
        break;
    }
}


void Octree::split(OctreeNode* _node) {
    if (_node->children[0] || _node->depth >= depth_limit) return; //if node already has children or at limit
    
    PhysicsServer3D* phys3 = PhysicsServer3D::get_singleton();
    
    Vector3 half_size = _node->bounds.size * 0.5f;

    for (int i = 0; i < 8; ++i) {
        Vector3 offset(
            (i & 1) ? half_size.x : 0.f,
            (i & 2) ? half_size.y : 0.f,
            (i & 4) ? half_size.z : 0.f
        );
        //(000) = left, bottom, front
        //(111) = right, top, back

        AABB child_bounds(_node->bounds.position + offset, half_size);
        //min = pos
        //max = pos + size

        RID area_rid = phys3->area_create();
        RID box_rid = phys3->box_shape_create();
        
        //box shape wants half extents
        phys3->shape_set_data(box_rid, child_bounds.size * 0.5f);
        phys3->area_add_shape(area_rid, box_rid);

        //setting area position to middle of aabb
        Transform3D transform(Basis(), child_bounds.position + child_bounds.size * 0.5f);
        phys3->area_set_transform(area_rid, transform);

        //initialilze child
        _node->children[i] = std::make_unique<OctreeNode>(
            _node,
            child_bounds,
            area_rid,
            box_rid
        );

        area_map[area_rid] = _node->children[i].get();

        Callable insert_callback = Callable(this, "insert").bind(area_rid);

        //activate area
        phys3->area_set_monitor_callback(area_rid, insert_callback);
        phys3->area_set_space(area_rid, get_world_3d()->get_space());
    }

    //disable the parent node physics
    phys3->area_set_space(_node->area_rid, RID());
    _node->count = 0;

    //TEMP
    print_line("split octree");

    update_debug();
}

void Octree::merge(OctreeNode* _node) {
    if (!_node || !_node->children[0]) return;
    
    //recursively remove children
    for (int i = 0; i < 8; ++i) {
        std::unique_ptr<OctreeNode>& current_child = _node->children[i];

        merge(current_child.get());
        area_map.erase(current_child->area_rid);
        current_child.reset();
    }
    
    PhysicsServer3D::get_singleton()->area_set_space(_node->area_rid, get_world_3d()->get_space());


    //TEMP
    print_line("merged octree");

    update_debug();
}
