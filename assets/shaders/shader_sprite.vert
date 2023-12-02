#version 330 core
layout (location = 0) in vec3 cubeCoords;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 model;

layout(std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};


out vec2 TexCoord;


void main()
{
	gl_Position = model * vec4(cubeCoords, 1.0);

	TexCoord = aTexCoord;
}