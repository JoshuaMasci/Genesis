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
	if (material.albedo_uv > -1) 
	{
		albedo *= texture(material.albedo_texture, material.albedo_uv == 0 ? frag_uv0 : frag_uv1);
	}
	return albedo;
};
uniform Material material;

struct CameraView
{
	vec2 framebuffer_size;
	sampler2D framebuffer;
};

vec4 getCameraView(CameraView view)
{
	vec2 uv = gl_FragCoord.xy / view.framebuffer_size;
	return texture(view.framebuffer, uv);  
};
uniform CameraView camera_view;

layout(location = 0) out vec4 out_color;
void main()
{
	vec4 view_color = getCameraView(camera_view);
	view_color.xyz = view_color.xyz * environment.ambient_light;
	vec4 window_color = getAlbedo(material);
	
	view_color.xyz *= view_color.w;
	window_color.xyz *= window_color.w;
	
	out_color.w = window_color.w + (view_color.w * (1.0 - window_color.w));
	out_color.xyz = window_color.xyz + (view_color.xyz * (1.0 - window_color.w));
}