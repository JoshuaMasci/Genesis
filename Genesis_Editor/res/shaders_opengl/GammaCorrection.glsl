#version 450

uniform float gamma;
layout(rgba32f) uniform image2D target;

layout (local_size_x = 1, local_size_y = 1) in;
void main()
{
	ivec2 image_pos = ivec2(gl_GlobalInvocationID.xy);
	vec4 value = imageLoad(target, image_pos);
	value.rgb = pow(value.rgb, vec3(1.0/gamma));
	imageStore(target, image_pos, value);
}