#version 460 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texcoord;
layout (location = 3) in vec4 in_joints;
layout (location = 4) in vec4 in_weights;

uniform mat4 transform;

layout(std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};

out vec3 normal;
out vec2 texcoord;

const int MaxBones = 200; // looks like too much

layout(std140) uniform BoneData {
    mat4 bone_transforms[MaxBones];
} bones;

void main() {
    vec4 world_pos = transform * vec4(in_position, 1.0);

    vec4 final_pos = vec4(0.0);
    for (int i = 0; i < 4; ++i) {
        int bone_index = int(in_joints[i]);
        float weight = in_weights[i];
        final_pos += bone_transforms[bone_index] * vec4(in_position, 1.0) * weight;
    }

    gl_Position = projection * view * worldPos;

    normal = normalize(mat3(transform) * in_normal);
    texcoord = in_texcoord;
}