#pragma once

#include "Genesis/Resource/Mesh.hpp"

namespace Genesis
{
	struct Material2 {};

	struct MeshComponent
	{
		Mesh* mesh;
		Material2 material;
	};
}