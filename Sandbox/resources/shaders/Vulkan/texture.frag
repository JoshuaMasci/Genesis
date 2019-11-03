#version 450

layout(location = 0) in vec3 frag_normal;
layout(location = 1) in vec2 frag_uv;

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
	return frag_normal;
}

vec4 calcLightColor(vec3 normal)
{
	return vec4(environment.ambient_light, 1.0);
}

#include "frag_main.slib"
