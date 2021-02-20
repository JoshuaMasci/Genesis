#pragma once

namespace genesis_engine
{
	struct MeshVertex
	{
		vec3f position;
		vec3f normal;
		vec3f tangent;
		vec3f bitangent;
		vec2f uv;
	};

	struct MeshVertexAnimated
	{
		vec3f position;
		vec3f normal;
		vec3f tangent;
		vec3f bitangent;
		vec2f uv;

		vec4f joints;
		vec4f weights;
	};
}