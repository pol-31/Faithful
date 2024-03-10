#version 460 core
layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_texcoord;

uniform mat3 in_position_transform;
uniform mat3 in_texcoord_transform;

out vec2 texcoord;

void main() {
    gl_Position = vec4(in_position_transform * vec3(in_position, 1.0f), 1.0);
    texcoord = (in_texcoord_transform * vec3(in_texcoord, 1.0f)).xy;
}
