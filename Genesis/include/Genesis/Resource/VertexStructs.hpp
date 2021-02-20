#pragma once

namespace Genesis
{
	struct MeshVertex
	{
		vector3F position;
		vector3F normal;
		vector3F tangent;
		vector3F bitangent;
		vector2F uv;
	};

	struct MeshVertexAnimated
	{
		vector3F position;
		vector3F normal;
		vector3F tangent;
		vector3F bitangent;
		vector2F uv;

		vector4F joints;
		vector4F weights;
	};
}