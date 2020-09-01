#pragma once

#include "Genesis/Resource/Mesh.hpp"
#include "Genesis/Resource/Material.hpp"

namespace Genesis
{
	struct MeshComponent
	{
		Mesh* mesh = nullptr;
		Material* material = nullptr;
	};
}