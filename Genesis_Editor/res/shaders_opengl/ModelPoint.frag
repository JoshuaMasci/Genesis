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
	vec3 albedo = full_albedo.xyz;
	vec2 metallic_roughness = getMetallicRoughness(material);
	float metallic = metallic_roughness.x;
	float roughness = metallic_roughness.y;

	vec3 normal = getNormal(material);
	
	vec3 light_direction = normalize(point_light.position - frag_world_pos);
	vec3 camera_direction = normalize(environment.camera_position - frag_world_pos);
	
	//PBR
	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, albedo.xyz, metallic);

	// reflectance equation
	float distance = length(point_light.position - frag_world_pos) / point_light.range;
	float attenuation = (point_light.attenuation.x /  distance) + (point_light.attenuation.y / (distance * distance));
	vec3 radiance = (point_light.base.color * point_light.base.intensity) * attenuation;      
	
	vec3 half_vector = normalize(camera_direction + light_direction);

	vec3 Lo = vec3(0.0);
	{
		// cook-torrance brdf
        float NDF = DistributionGGX(normal, half_vector, roughness);        
        float G   = GeometrySmith(normal, camera_direction, light_direction, roughness);      
        vec3 F    = fresnelSchlick(max(dot(half_vector, camera_direction), 0.0), F0);
		
		vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(normal, camera_direction), 0.0) * max(dot(normal, light_direction), 0.0);
        vec3 specular     = numerator / max(denominator, 0.001);  
            
        // add to outgoing radiance Lo
        float NdotL = max(dot(normal, light_direction), 0.0);                
        Lo = (kD * albedo / PI + specular) * radiance * NdotL; 
	}
	

	
	out_color = vec4(Lo, 0.0);
}