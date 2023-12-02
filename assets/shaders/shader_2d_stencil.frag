#version 330 core

in vec2 TexCoord;

layout(location = 0) out vec4 fragColor0;
layout(location = 1) out vec4 fragColor1;

uniform sampler2D texture1;
uniform int id;

void main()
{
    gl_FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);//texture(texture1, TexCoord);
}