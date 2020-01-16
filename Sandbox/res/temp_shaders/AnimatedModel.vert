#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec3 in_tanget;
layout(location = 4) in vec3 in_bitanget;
layout(location = 5) in uvec4 in_joint_ids;
layout(location = 6) in vec4 in_joint_weights;

layout(push_constant) uniform Matrices
{
	mat4 mvp;
} matrices;

layout(binding = 0) uniform Bones
{
	mat4 bones[64];
} bones;

layout(location = 0) out vec2 out_uv;

void main()
{
	mat4 bone_transform = bones.bones[in_joint_ids[0]] * in_joint_weights[0];
    bone_transform += bones.bones[in_joint_ids[1]] * in_joint_weights[1];
    bone_transform += bones.bones[in_joint_ids[2]] * in_joint_weights[2];
    bone_transform += bones.bones[in_joint_ids[3]] * in_joint_weights[3];
	
	gl_Position = matrices.mvp * (bone_transform * vec4(in_position, 1.0));
	
	out_uv = in_uv;
}