#version 450

#include "lighting.slib"

layout(location = 0) in vec3 frag_world_pos;
layout(location = 1) in vec2 frag_uv;
layout(location = 2) in mat3 tangent_space_matrix;

layout(binding = 0) uniform Environment
{
	vec3 eye_pos;
	vec3 ambient_light;
	float height_scale;
} environment;


layout(binding = 1) uniform sampler2D albedo_texture;
layout(binding = 2) uniform sampler2D height_texture;

vec2 calcUV()
{
	vec3 view_direction = normalize(environment.eye_pos - frag_world_pos);
	view_direction = transpose(tangent_space_matrix) * view_direction;
	return ParallaxOcclusionMapping(height_texture, frag_uv, view_direction, environment.height_scale);
}

vec4 calcBaseColor(vec2 uv)
{
	return texture(albedo_texture, uv);
}

vec3 calcNormal(vec2 uv)
{
	return vec3(0.0, 0.0, 0.0);
}

vec4 calcLightColor(vec3 normal)
{
	return vec4(environment.ambient_light, 1.0);
}

#include "frag_main.slib"
