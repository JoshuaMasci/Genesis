#version 450

layout(location = 0) in vec3 in_normal;
layout(location = 1) in vec2 in_tex;

layout(location = 0) out vec4 out_color;

layout(binding = 1) uniform sampler2D albedo_texture;

void main()
{
    out_color = texture(albedo_texture, in_tex);
}