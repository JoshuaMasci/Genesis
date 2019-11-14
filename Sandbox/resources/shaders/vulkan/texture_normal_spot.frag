#version 450

#include "lighting.slib"

layout(location = 0) in vec3 frag_world_pos;
layout(location = 1) in vec2 frag_uv;
layout(location = 2) in mat3 tangent_space_matrix;

layout(binding = 0) uniform Environment
{
	vec3 eye_pos;
	SpotLight spot;
	float height_scale;
} environment;

layout(binding = 1) uniform sampler2D albedo_texture;
layout(binding = 2) uniform sampler2D normal_texture;
layout(binding = 3) uniform sampler2D height_texture;

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
	vec3 normal = texture(normal_texture, uv).xyz;
	normal = (normal * 2.0) - 1.0f;
	return tangent_space_matrix * normal;
}

vec4 calcLightColor(vec3 normal)
{
	return CalcSpotLight(environment.spot, normal, frag_world_pos, environment.eye_pos);
}

#include "frag_main.slib"
