#version 450

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec4 in_color;

layout(location = 0) out vec2 uv;
layout(location = 1) out vec4 color;

struct Offset
{ 
	vec2 uScale; 
	vec2 uTranslate;
};

uniform Offset offset;

void main()
{
	gl_Position = vec4(in_position  * offset.uScale + offset.uTranslate, 0.0, 1.0);
	uv = in_uv;
	color = in_color;
}