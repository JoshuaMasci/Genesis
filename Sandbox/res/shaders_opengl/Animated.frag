#version 450

layout(location = 0) in vec2 frag_uv;

uniform sampler2D in_texture;

layout(location = 0) out vec4 out_color;
void main()
{
	//out_color = texture(in_texture, frag_uv);
	out_color = vec4(0.0, 1.0, 1.0, 1.0);
}