#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;


layout(location = 0) out vec3 frag_world_pos;
layout(location = 1) out vec3 frag_normal;
layout(location = 2) out vec2 frag_uv;

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
	
	frag_normal = matrices.normal * in_normal;
	
	frag_uv = in_uv;
}