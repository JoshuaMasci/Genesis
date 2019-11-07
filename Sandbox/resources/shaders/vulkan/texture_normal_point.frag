#version 450

#include "lighting.slib"

layout(location = 0) in vec3 frag_world_pos;
layout(location = 1) in vec2 frag_uv;
layout(location = 2) in mat3 tangent_space_matrix;

layout(binding = 0) uniform sampler2D albedo_texture;
layout(binding = 1) uniform sampler2D normal_texture;
layout(binding = 2) uniform Lights
{
	vec3 eye_pos;
	PointLight point;
} lights;

vec4 calcBaseColor()
{
	return texture(albedo_texture, frag_uv);
}

vec3 calcNormal()
{
	vec3 normal = texture(normal_texture, frag_uv).xyz;
	normal = (normal * 2.0) - 1.0f;
	return tangent_space_matrix * normal;
}

vec4 calcLightColor(vec3 normal)
{
	return CalcPointLight(lights.point, normal, frag_world_pos, lights.eye_pos);
}

#include "frag_main.slib"
