#version 450

layout(push_constant) uniform Matrices
{
	mat4 ModelViewProj;
} matrices;

layout(location = 0) in vec3 in_position;

void main()
{
    gl_Position = matrices.ModelViewProj * vec4(in_position, 1.0);
}