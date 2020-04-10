#version 450

layout(location = 0) in vec3 frag_world_pos;
layout(location = 1) in vec2 frag_uv;
layout(location = 2) in mat3 frag_tangent_space;

struct Environment
{
	vec3 camera_position;
	vec3 ambient_light;
	mat4 view_projection_matrix;
};
uniform Environment environment;

struct Material
{
	vec4 albedo;
	vec4 pbr_values;
	bvec4 has_textures1;
	bvec4 has_textures2;
};
uniform Material material;
uniform sampler2D material_textures[8];

vec2 calcUV()
{
	/*if(material.has_textures1.z)
	{
		vec3 view_direction = normalize(environment.camera_position - frag_world_pos);
		view_direction = transpose(frag_tangent_space) * view_direction;
		return ParallaxOcclusionMapping(material_textures[2], frag_uv, view_direction, material.pbr_values.w);
	}
	else*/
	{
		return frag_uv;
	}
}

vec4 calcBaseColor(vec2 uv)
{
	if(material.has_textures1.x)
	{
		return texture(material_textures[0], uv);
	}
	else
	{
		return material.albedo;
	}
}

vec3 calcNormal(vec2 uv)
{
	if(material.has_textures1.y)
	{
		vec3 normal = texture(material_textures[1], uv).xyz;
		normal = (normal * 2.0) - 1.0f;
		return frag_tangent_space * normal;
	}
	else
	{
		return frag_tangent_space[2];
	}
}

vec4 calcLightColor(vec3 normal)
{
	return vec4(environment.ambient_light, 1.0);
}

layout(location = 0) out vec4 out_color;
void main()
{
	vec2 uv = calcUV();
	vec4 base_color = calcBaseColor(uv);
	vec3 normal = calcNormal(uv);
	vec4 light_color = calcLightColor(normal);
	out_color = base_color * light_color;
}