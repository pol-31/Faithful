#version 460 core

layout (location = 0) in vec2 in_position;

uniform mat3 in_position_transform;

void main() {
    gl_Position = vec4(in_position_transform * vec3(in_position, 1.0f), 1.0);
}
