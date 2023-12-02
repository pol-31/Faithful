#version 330 core

in vec2 TexCoord;

layout(location = 0) out vec4 fragColor0;
layout(location = 1) out vec4 fragColor1;

uniform sampler2D texture1;
uniform float id;

out vec3 FragColor;

void main()
{
    FragColor = vec3(id, id, id);
}