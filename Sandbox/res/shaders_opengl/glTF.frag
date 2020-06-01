#version 450

layout(location = 0) in vec3 frag_world_pos;
layout(location = 1) in vec3 frag_normal;
layout(location = 2) in vec2 frag_uv0;
layout(location = 3) in vec2 frag_uv1;

struct Environment
{
	vec3 ambient_light;
	vec3 camera_position;
	mat4 view_projection_matrix;
};
uniform Environment environment;

struct Material
{
	vec4 albedo;
	vec2 metallic_roughness;
	vec4 emissive;
	
	int albedo_uv;
	int metallic_roughness_uv;
	int normal_uv;	
	int occlusion_uv;
	int emissive_uv;
};
uniform Material material;
// albedo_texture
// normal_texture
// metallic_roughness_texture
// occlusion_texture
// emissive_texture
uniform sampler2D material_textures[5];

layout(location = 0) out vec4 out_color;
void main()
{
	vec4 albedo = material.albedo;
	if (material.albedo_uv > -1) 
	{
		albedo *= texture(material_textures[0], material.albedo_uv == 0 ? frag_uv0 : frag_uv1);
	}
	out_color = albedo;
}