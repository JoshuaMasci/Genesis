#version 450

layout(location = 0) in vec3 frag_world_pos;
layout(location = 1) in vec3 frag_normal;
layout(location = 2) in vec2 frag_uv;

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform sampler2D albedo_texture;

layout(binding = 1) uniform DirectionalLight
{
	vec3 color;
    float intensity;
	vec3 direction;
} light;


vec4 CalcLight(vec3 base_color, float base_intensity, vec3 direction, vec3 normal, vec3 worldPos)
{
    float diffuseFactor = dot(normal, -direction);
    
    vec4 diffuseColor = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 specularColor = vec4(0.0, 0.0, 0.0, 0.0);
    
    if(diffuseFactor > 0.0)
    {
        diffuseColor = vec4(base_color, 1.0) * base_intensity * diffuseFactor;
        
		vec3 eyePos = vec3(0.0, 0.0, 0.0); //EyePos is always the origin
        vec3 directionToEye = normalize(eyePos - worldPos);
        //vec3 reflectDirection = normalize(reflect(direction, normal));
        vec3 halfDirection = normalize(directionToEye - direction);
        
        float specularFactor = dot(halfDirection, normal);
        //specularFactor = pow(specularFactor, specularPower);
		specularFactor = pow(specularFactor, 96.078431);
        
        if(specularFactor > 0.0)
        {
			specularColor = vec4(base_color, 1.0) * 0.5 * specularFactor;
        }
    }
    
    return diffuseColor + specularColor;
}

void main()
{
	vec4 light_value = CalcLight(light.color, light.intensity, light.direction, frag_normal, frag_world_pos);
    out_color = texture(albedo_texture, frag_uv) * light_value;
}