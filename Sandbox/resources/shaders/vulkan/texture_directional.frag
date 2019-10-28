#version 450

#include "lighting.slib"

layout(location = 0) in vec3 frag_world_pos;
layout(location = 1) in vec3 frag_normal;
layout(location = 2) in vec2 frag_uv;

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform sampler2D albedo_texture;

layout(binding = 1) uniform Lights
{
	vec3 eye_pos;
	DirectionalLight directional;
} lights;

void main()
{
	vec4 light_value = CalcDirectionalLight(lights.directional, frag_normal, frag_world_pos, lights.eye_pos);
    out_color = texture(albedo_texture, frag_uv) * light_value;
}