#version 450

#include "lighting.slib"

layout(location = 0) in vec3 frag_world_pos;
layout(location = 1) in vec3 frag_normal;
layout(location = 2) in vec2 frag_uv;

layout(binding = 0) uniform sampler2D albedo_texture;
layout(binding = 1) uniform Lights
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
	return frag_normal;
}

vec4 calcLightColor(vec3 normal)
{
	return CalcPointLight(lights.point, normal, frag_world_pos, lights.eye_pos);
}

#include "frag_main.slib"
