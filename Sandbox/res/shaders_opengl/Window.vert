#version 450

layout(location = 0) in vec3 in_position;

struct Matrices
{
	mat4 model;
	mat4 view_projection_matrix;
};
uniform Matrices matrices;

void main()
{
	vec4 vert_position = vec4(in_position, 1.0);
	vert_position = matrices.model * vert_position;
	vert_position = matrices.view_projection_matrix * vert_position;
    gl_Position = vert_position;
}