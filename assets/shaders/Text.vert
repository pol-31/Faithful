#version 460 core
layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_texcoord;
layout (location = 2) in vec2 in_texcoord_font; // TODO: integrate

out vec2 texcoord;
uniform vec2 texcoord_offset;

// TODO we forgot about bitmap_coordinates

void main() {
    gl_Position = vec4(in_position, 1.0f, 1.0);
    texcoord = in_texcoord + texcoord_offset; // wtf, TODO: need to precalc
}