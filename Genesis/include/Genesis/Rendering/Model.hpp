#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/VertexInputDescription.hpp"

namespace Genesis
{
	struct AnimatedModel
	{
		struct AnimatedModelVertex
		{
			vector3F position;
			vector2F uv;
			vector3F normal;
			vector3F tangent;
			vector3F bitangent;
			vector4U joint_ids;
			vector4F joint_weights;
		};

		static VertexInputDescription getVertexDescription()
		{
			return VertexInputDescription({
					{"in_position", VertexElementType::float_3},
					{"in_uv", VertexElementType::float_2},
					{"in_normal", VertexElementType::float_3},
					{"in_tangent", VertexElementType::float_3},
					{"in_bitangent", VertexElementType::float_3},
					{"in_joint_ids", VertexElementType::uint32_4},
					{"in_joint_weights", VertexElementType::float_4},
				});
		};
	};
}