#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_tex;

layout(location = 0) out vec3 frag_normal;
layout(location = 1) out vec2 frag_tex;

layout(push_constant) uniform Matrices
{
	mat4 mvp;
} matrices;

void main()
{
    gl_Position = matrices.mvp * vec4(in_position, 1.0);
	
	frag_normal = in_normal;
	frag_tex = in_tex;
}