#version 460 core
layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_background_texcoord;

out vec2 bitmap_texcoord;

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(in_position, 0.0, 1.0);
    bitmap_texcoord = in_bitmap_texcoord;
}
