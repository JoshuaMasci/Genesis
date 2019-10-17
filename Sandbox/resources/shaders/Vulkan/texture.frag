#version 450

layout(location = 0) in vec3 frag_normal;
layout(location = 1) in vec2 frag_uv;

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform sampler2D albedo_texture;

layout(binding = 1) uniform Environment
{
	vec3 ambient_light;
} environment;

void main()
{
    out_color = texture(albedo_texture, frag_uv) * vec4(environment.ambient_light, 1.0);
}