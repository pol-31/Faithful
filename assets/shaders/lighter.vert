#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 model;

layout(std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};

out vec2 TexCoord;

void main() {
  gl_Position = projection * view * model * vec4(aPosition, 1.0f);
  TexCoord = aTexCoord;
};