#version 450

layout(location = 0) in vec3 frag_world_pos;
layout(location = 1) in vec2 frag_uv;
layout(location = 2) in mat3 frag_tangent_space;

#include "Environment.slib"
uniform Environment environment;

#include "Material.slib"
uniform Material material;

layout(location = 0) out vec4 out_color;
void main()
{
	vec4 color = getAlbedo(material);
	color.xyz = color.xyz * environment.ambient_light * getOcclusion(material);	
	out_color = color + getEmissive(material);
}

