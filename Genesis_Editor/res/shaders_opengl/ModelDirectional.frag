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
	vec3 normal = getNormal(material);
	vec3 view = normalize(environment.camera_position - frag_world_pos);
	
	vec4 full_albedo = getAlbedo(material);
	vec3 albedo = full_albedo.xyz;
	vec2 metallic_roughness = getMetallicRoughness(material);
	float metallic = metallic_roughness.x;
	float roughness = metallic_roughness.y;
	
	//PBR
	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, albedo.xyz, metallic);
	
	// reflectance equation
	vec3 Lo = vec3(0.0);
	{
		vec3 L = normalize(-directional_light.direction);
		vec3 H = normalize(view + L);
		vec3 radiance = directional_light.base.color * directional_light.base.intensity;
		
		// cook-torrance brdf
        float NDF = DistributionGGX(normal, H, roughness);        
        float G   = GeometrySmith(normal, view, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, view), 0.0), F0);
		
		vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(normal, view), 0.0) * max(dot(normal, L), 0.0);
        vec3 specular     = numerator / max(denominator, 0.001);  
            
        // add to outgoing radiance Lo
        float NdotL = max(dot(normal, L), 0.0);                
        Lo = (kD * albedo / PI + specular) * radiance * NdotL; 
	}
	
	out_color.xyz = Lo;
	out_color.w = 0.0;
}