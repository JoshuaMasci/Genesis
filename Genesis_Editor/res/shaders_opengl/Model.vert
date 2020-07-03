#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv0;
layout(location = 3) in vec2 in_uv1;
layout(location = 4) in vec4 in_joint0;
layout(location = 5) in vec4 in_weight0;

layout(location = 0) out vec3 frag_world_pos;
layout(location = 1) out vec3 frag_normal;
layout(location = 2) out vec2 frag_uv0;
layout(location = 3) out vec2 frag_uv1;

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
};
uniform Matrices matrices;

void main()
{
	vec4 vert_position = vec4(in_position, 1.0);
	vec3 vert_normal = in_normal;
	
	vert_position = matrices.model * vert_position;
	vert_normal = normalize(transpose(inverse(mat3(matrices.model))) * in_normal);
	
    gl_Position = environment.view_projection_matrix * vert_position;
	
	frag_world_pos = vert_position.xyz;
	frag_normal = vert_normal;
	
	frag_uv0 = in_uv0;
	frag_uv1 = in_uv1;
}