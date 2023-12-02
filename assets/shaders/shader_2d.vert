#version 330 core
layout (location = 0) in vec2 rectCoords;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat3 transform;

void main()
{
	gl_Position = vec4(rectCoords, 0.0f, 1.0f);

	TexCoord = aTexCoord;
}