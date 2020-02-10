#version 450

layout(location = 0) in vec3 frag_world_pos;
layout(location = 1) in vec2 frag_uv;
layout(location = 2) in mat3 frag_tangent_space;

#include "components/lighting.slib"

#include "components/environment.slib"

#include "components/material.slib"

layout (constant_id = 0) const bool has_albedo_map = false;
layout (constant_id = 1) const bool has_normal_map = false;
layout (constant_id = 2) const bool has_height_map = false;
layout (constant_id = 3) const bool has_metallic_map = false;
layout (constant_id = 4) const bool has_roughness_map = false;
layout (constant_id = 5) const bool has_ambient_occlusion_map = false;

vec2 calcUV()
{
	if(has_height_map)
	{
		vec3 view_direction = normalize(environment.camera_position - frag_world_pos);
		view_direction = transpose(frag_tangent_space) * view_direction;
		return ParallaxOcclusionMapping(height_map, frag_uv, view_direction, material.pbr_values.w);
	}
	else
	{
		return frag_uv;
	}
}

vec4 calcBaseColor(vec2 uv)
{
	if(has_albedo_map)
	{
		return texture(albedo_map, uv);
	}
	else
	{
		return material.albedo;
	}
}

vec3 calcNormal(vec2 uv)
{
	if(has_normal_map)
	{
		vec3 normal = texture(normal_map, uv).xyz;
		normal = (normal * 2.0) - 1.0f;
		return frag_tangent_space * normal;
	}
	else
	{
		return frag_tangent_space[2];
	}
}

vec4 calcLightColor(vec3 normal)
{
	return vec4(environment.ambient_light, 1.0);
}

#include "frag_main.slib"
