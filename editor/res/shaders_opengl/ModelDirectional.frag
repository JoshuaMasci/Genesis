#version 450

layout(location = 0) in vec3 frag_world_pos;
layout(location = 1) in vec2 frag_uv;
layout(location = 2) in mat3 frag_tangent_space;

#include "Environment.slib"
uniform Environment environment;

#include "Material.slib"
uniform Material material;

#include "Lighting.slib"
uniform DirectionalLight directional_light;

#include "Pbr.slib"

layout(location = 0) out vec4 out_color;
void main()
{
	vec4 full_albedo = getAlbedo(material);
	vec3 normal = getNormal(material);	
	
	vec3 frag_to_cam_dir = normalize(environment.camera_position - frag_world_pos);
	vec3 frag_to_light_dir = -directional_light.direction;
	
	vec3 albedo = full_albedo.xyz;
	vec2 metallic_roughness = getMetallicRoughness(material);
	vec3 radiance = directional_light.base.color * directional_light.base.intensity;

	PbrMaterial material = PbrMaterial(albedo, metallic_roughness.x, metallic_roughness.y, metallic_roughness.y * metallic_roughness.y);
	out_color.xyz = calcDirectLight(material, normal, frag_to_cam_dir, frag_to_light_dir, radiance);
	out_color.w = full_albedo.w;
}