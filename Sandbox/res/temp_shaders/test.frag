#version 450

layout(location = 0) in vec3 frag_color;

layout(location = 0) out vec4 out_color;

layout(push_constant) uniform Offset
{ 
	vec3 color;
} offset;

void main() {
    out_color = vec4(offset.color, 1.0);
}