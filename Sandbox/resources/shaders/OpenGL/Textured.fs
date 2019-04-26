#version 330 core

layout (location = 0) out vec4 albedo;

in vec3 out_Normal;
in vec2 out_TexCoord;
in vec3 out_FragPos;

uniform sampler2D texture1;

void main(void) 
{
	albedo = texture(texture1, out_TexCoord);
}
