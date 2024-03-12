#version 460 core
layout (location = 0) in vec2 in_bitmap_texcoord;
layout (location = 1) in vec2 in_position;
layout (location = 2) in vec2 in_background_texcoord;

out vec2 bitmap_texcoord;
out vec2 background_texcoord;

void main() {
    gl_Position = vec4(in_position, 0.0, 1.0);
    bitmap_texcoord = in_bitmap_texcoord;
    background_texcoord = in_background_texcoord;
}
