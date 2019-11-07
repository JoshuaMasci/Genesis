#version 450

layout(location = 0) in vec3 frag_world_pos;
layout(location = 1) in vec2 frag_uv;
layout(location = 2) in mat3 tangent_space_matrix;

layout(binding = 0) uniform sampler2D albedo_texture;
layout(binding = 1) uniform Environment
{
	vec3 ambient_light;
} environment;

vec4 calcBaseColor()
{
	return texture(albedo_texture, frag_uv);
}

vec3 calcNormal()
{
	return vec3(0.0, 0.0, 0.0);
}

vec4 calcLightColor(vec3 normal)
{
	return vec4(environment.ambient_light, 1.0);
}

#include "frag_main.slib"
