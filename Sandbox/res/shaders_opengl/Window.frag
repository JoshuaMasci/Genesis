#version 450

uniform sampler2D framebuffer;
uniform vec2 screen_size;

layout(location = 0) out vec4 out_color;
void main()
{
	vec2 uv = gl_FragCoord.xy / screen_size;
	out_color = texture(framebuffer, uv);  
}