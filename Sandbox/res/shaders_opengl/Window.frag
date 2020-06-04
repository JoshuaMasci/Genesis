#version 450

uniform sampler2D framebuffer;
uniform vec2 screen_size;

layout(location = 0) out vec4 out_color;
void main()
{
	vec2 uv = gl_FragCoord.xy / screen_size;
	out_color = texture(framebuffer, uv);
	//out_color = vec4(uv, 0.0, 1.0);
	
	//if(uv.y < 0.5)
    //    out_color = vec4(1.0, 0.0, 0.0, 1.0);
    //else
     //   out_color = vec4(0.0, 1.0, 0.0, 1.0);   
}