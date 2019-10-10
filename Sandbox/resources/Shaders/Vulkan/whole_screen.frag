#version 450

layout(location = 0) in vec2 out_uv;

layout(binding = 0) uniform sampler2D in_texture;

layout(location = 0) out vec4 out_color;

void main()
{
    out_color = texture(in_texture, out_uv);
}