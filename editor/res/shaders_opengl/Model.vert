#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_tangent;
layout(location = 3) in vec3 in_bitangent;
layout(location = 4) in vec2 in_uv;

layout(location = 0) out vec3 frag_world_pos;
layout(location = 1) out vec2 frag_uv;
layout(location = 2) out mat3 frag_tangent_space;

struct Environment
{
	vec3 ambient_light;
	vec3 camera_position;
	mat4 view_projection_matrix;
};
uniform Environment environment;

struct Matrices
{
	mat4 model;
	mat3 normal;
};
uniform Matrices matrices;

void main()
{
	vec4 vert_position = matrices.model * vec4(in_position, 1.0);	
    gl_Position = environment.view_projection_matrix * vert_position;
	frag_world_pos = vert_position.xyz;
	
	frag_uv = in_uv;
	
	vec3 T = normalize(matrices.normal * in_tangent);
	vec3 B = normalize(matrices.normal * in_bitangent);	
	vec3 N = normalize(matrices.normal * in_normal);
	frag_tangent_space = mat3(T, B, N);
}