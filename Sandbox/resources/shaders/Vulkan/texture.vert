#version 450

layout(push_constant) uniform Matrices
{
	mat4 ModelViewProj;
} matrices;

layout(std140, set = 0, binding = 0) uniform DirectionalLight
{
	vec3 color;
} directional_light;

layout(std140, set = 1, binding = 0) uniform PointLight
{
	vec3 color;
	vec3 pos;
	float range;
} point_light;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_tex;

layout(location = 0) out vec3 frag_normal;
layout(location = 1) out vec2 frag_tex;

void main()
{
    gl_Position = matrices.ModelViewProj * vec4(in_position, 1.0);
	
	frag_normal = in_normal;
	frag_tex = in_tex;
}