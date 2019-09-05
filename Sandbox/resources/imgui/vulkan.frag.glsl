#version 450

layout(binding = 0) uniform sampler2D texture_atlas;

layout(location = 0) in vec2 uv;
layout(location = 1) in vec4 color;

layout(location = 0) out vec4 out_color;

void main()
{
    out_color = color * texture(texture_atlas, uv);
}