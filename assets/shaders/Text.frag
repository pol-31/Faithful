#version 460 core

in vec2 bitmap_texcoord;

uniform sampler2D tex_bitmap;

uniform vec4 color;
uniform float brightness;

out vec4 FragColor;

void main() {
    vec4 texel = vec4(1.0, 1.0, 1.0, texture(tex_bitmap, bitmap_texcoord).r);
    texel *= color;
    texel *= brightness;
    FragColor = texel;
}
