#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;

layout(push_constant) uniform Matrices
{
	vec4 color;
	mat4 view_projection;
	mat4 model;
} matrices;

layout(location = 0) out vec2 out_uv;
layout(location = 1) out vec4 out_color;

void main()
{	
	gl_Position = matrices.view_projection * (matrices.model * vec4(in_position, 1.0));
	
	out_uv = in_uv;
	out_color = matrices.color;
}