#version 450

layout(push_constant) uniform Matrices
{
	mat4 ModelViewProj;
} matrices;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_tex;

layout(location = 0) out vec3 frag_normal;

void main()
{
    gl_Position = matrices.ModelViewProj * vec4(in_position, 1.0);
	
	frag_normal = vec3(in_tex, 1.0);
}