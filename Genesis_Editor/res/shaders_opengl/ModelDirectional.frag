#version 450

layout(location = 0) in vec3 frag_world_pos;
layout(location = 1) in vec2 frag_uv;
layout(location = 2) in mat3 frag_tangent_space;

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
	int normal_uv;
	int metallic_roughness_uv;
	int occlusion_uv;
	int emissive_uv;
	
	sampler2D albedo_texture;
	sampler2D normal_texture;
	sampler2D metallic_roughness_texture;
	sampler2D occlusion_texture;
	sampler2D emissive_texture;
};

vec4 getAlbedo(Material material)
{
	vec4 albedo = material.albedo;
	//if (material.albedo_uv > -1) 
	//{
	//	albedo *= texture(material.albedo_texture, material.albedo_uv == 0 ? frag_uv0 : frag_uv1);
	//}
	return albedo;
};
uniform Material material;

struct BaseLight
{
    vec3 color;
    float intensity;
};

struct DirectionalLight
{
	BaseLight base;
	vec3 direction;
};

vec4 CalcLight(BaseLight base, vec3 direction, vec3 normal, vec3 worldPos)
{
    float diffuseFactor = dot(normal, -direction);
    
    vec4 diffuseColor = vec4(0.0, 0.0, 0.0, 0.0);
    
    if(diffuseFactor > 0.0)
    {
        diffuseColor = vec4(base.color, 1.0) * base.intensity * diffuseFactor;
    }
    
    return diffuseColor;
}

vec4 CalcDirectionalLight(DirectionalLight directional, vec3 normal, vec3 worldPos)
{
    return CalcLight(directional.base, directional.direction, normal, worldPos);
}

uniform DirectionalLight directional_light;

layout(location = 0) out vec4 out_color;
void main()
{
	vec4 color = getAlbedo(material);
	color.xyz = (color * CalcDirectionalLight(directional_light, frag_tangent_space[2], frag_world_pos)).xyz;
	out_color = color;
}