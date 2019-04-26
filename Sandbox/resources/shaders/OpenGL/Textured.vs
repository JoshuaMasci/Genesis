#version 330 core
layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_TexCoord;

out vec3 out_Normal;
out vec2 out_TexCoord;
out vec3 out_FragPos;

uniform mat4 MVP;
uniform mat4 model_matrix;
 
void main(void) 
{	
	out_Normal = (model_matrix * vec4(in_Normal, 0.0f)).xyz;
	out_TexCoord = in_TexCoord;
	out_FragPos = (model_matrix * vec4(in_Position, 1.0f)).xyz;
	
	gl_Position = MVP * vec4(in_Position, 1.0f);
}
