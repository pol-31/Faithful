#version 460 core

in vec2 texcoord;

uniform sampler2D tex_button;

uniform vec4 color;
uniform float brightness;

out vec4 FragColor;

void main() {
    vec4 texel = texture(tex_button, texcoord);
    texel *= color;
    texel *= brightness;
    FragColor = texel;
}