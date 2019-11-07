#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec3 in_tangent;

layout(location = 0) out vec3 frag_world_pos;
layout(location = 1) out vec2 frag_uv;
layout(location = 2) out mat3 tangent_space_matrix;

layout(push_constant) uniform Matrices
{
	mat4 mvp;
	mat4 model;
	mat3 normal;
} matrices;

void main()
{
    gl_Position = matrices.mvp * vec4(in_position, 1.0);
	frag_world_pos = (matrices.model * vec4(in_position, 1.0)).xyz;
	frag_uv = in_uv;
	
	vec3 N = normalize(matrices.normal * in_normal);
	vec3 T = normalize(matrices.normal * in_tangent);
	vec3 B = cross(N, T);	
	tangent_space_matrix = mat3(T, B, N);
}