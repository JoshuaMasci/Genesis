#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

layout(push_constant) uniform Matrices
{
	mat4 mvp;
} matrices;

void main()
{
    gl_Position = matrices.mvp * vec4(in_position, 1.0);
}