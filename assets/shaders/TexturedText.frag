#version 460 core

in vec2 bitmap_texcoord;
in vec2 background_texcoord;

uniform sampler2D tex_bitmap;
uniform sampler2D tex_background;

uniform vec4 color;
uniform float brightness;

out vec4 FragColor;

void main() {
    vec4 texel = texture(tex_background, background_texcoord);
    texel *= texture(tex_bitmap, bitmap_texcoord).r;
    texel *= color;
    texel *= brightness;
    FragColor = texel;
}
