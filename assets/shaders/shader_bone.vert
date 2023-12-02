#version 330 core
layout (location = 0) in vec3 cubeCoords;
layout (location = 1) in vec2 aTexCoord;
layout (location = 5) in vec4 Weights;
layout (location = 6) in ivec4 BoneIDs;

uniform mat4 model;

layout(std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};

out vec2 TexCoord;

const int MaxBones = 200;

layout(std140) uniform BoneData {
    mat4 boneTransforms[MaxBones];
} bones;

void main()
{
    mat4 bone_transform = bones.boneTransforms[BoneIDs[0]] * Weights[0];
    bone_transform += bones.boneTransforms[BoneIDs[1]] * Weights[1];
    bone_transform += bones.boneTransforms[BoneIDs[2]] * Weights[2];
    bone_transform += bones.boneTransforms[BoneIDs[3]] * Weights[3];

	gl_Position = projection * view * model * bone_transform * vec4(cubeCoords, 1.0f);

	TexCoord = aTexCoord;
}
