#version 450

#include "lighting.slib"

layout(location = 0) in vec3 frag_world_pos;
layout(location = 1) in vec3 frag_normal;
layout(location = 2) in vec2 frag_uv;

layout(binding = 0) uniform sampler2D albedo_texture;
layout(binding = 1) uniform sampler2D shadow_map;
layout(binding = 2) uniform Lights
{
	vec3 eye_pos;
	SpotLight spot;
	mat4 shadow_mv;
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
	vec4 light_space_pos = lights.shadow_mv * vec4(frag_world_pos, 1.0);
	return CalcSpotLight(lights.spot, normal, frag_world_pos, lights.eye_pos) * CalcShadow(shadow_map, light_space_pos);
}

#include "frag_main.slib"
