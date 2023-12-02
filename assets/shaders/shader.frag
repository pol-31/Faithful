#version 330 core

in vec2 TexCoord;

uniform sampler2D texture_albedo;
uniform int id;

void main()
{
    gl_FragColor = texture(texture_albedo, TexCoord);
}