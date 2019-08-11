#version 450

layout(set = 1, binding = 0) uniform sampler2D albedo_texture;

layout(location = 0) in vec3 in_normal;
layout(location = 1) in vec2 in_tex;

layout(location = 0) out vec4 out_color;

void main()
{
    out_color = texture(albedo_texture, in_tex);
}