#version 450

layout(location = 0) in vec3 frag_world_pos;
layout(location = 1) in vec2 frag_uv;
layout(location = 2) in mat3 frag_tangent_space;

#include "Environment.slib"
uniform Environment environment;

#include "Material.slib"
uniform Material material;

#include "Lighting.slib"
uniform PointLight point_light;

#include "Pbr.slib"

layout(location = 0) out vec4 out_color;
void main()
{	
	vec4 full_albedo = getAlbedo(material);
	vec3 normal = getNormal(material);	
	
	vec3 frag_to_cam_dir = normalize(environment.camera_position - frag_world_pos);
	vec3 frag_to_light_dir = normalize(point_light.position - frag_world_pos);
	
	vec3 albedo = full_albedo.xyz;
	vec2 metallic_roughness = getMetallicRoughness(material);
	
	float distance = length(point_light.position - frag_world_pos) / point_light.range;
	distance = min(distance, 1.0);
	float attenuation = (point_light.attenuation.x /  distance) + (point_light.attenuation.y / (distance * distance));
	attenuation = max(attenuation, 0.0001);
	vec3 radiance = (point_light.base.color * point_light.base.intensity) * attenuation; 

	PbrMaterial material = PbrMaterial(albedo, metallic_roughness.x, metallic_roughness.y, metallic_roughness.y * metallic_roughness.y);
	out_color.xyz = calcDirectLight(material, normal, frag_to_cam_dir, frag_to_light_dir, radiance);
	out_color.w = full_albedo.w;
}