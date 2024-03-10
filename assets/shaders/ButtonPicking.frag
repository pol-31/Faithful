#version 460 core

uniform uint object_id;

out uint out_object_id;

void main() {
    out_object_id = object_id;
}
