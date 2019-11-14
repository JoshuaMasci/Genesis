#version 450

#include "lighting.slib"

layout(location = 0) in vec3 frag_world_pos;
layout(location = 1) in vec3 frag_normal;
layout(location = 2) in vec2 frag_uv;

layout(binding = 0) uniform sampler2D albedo_texture;
layout(binding = 1) uniform Environment
{
	vec3 eye_pos;
	SpotLight spot;
} environment;

vec2 calcUV()
{
	return frag_uv;
}

vec4 calcBaseColor(vec2 uv)
{
	return texture(albedo_texture, uv);
}

vec3 calcNormal(vec2 uv)
{
	return frag_normal;
}

vec4 calcLightColor(vec3 normal)
{
	return CalcSpotLight(environment.spot, normal, frag_world_pos, environment.eye_pos);
}

#include "frag_main.slib"
