#pragma once

#include "Genesis/Resource/Mesh.hpp"
#include "Genesis/Resource/Material.hpp"

namespace Genesis
{
	struct MaterialMeshComponent
	{
		Mesh* mesh = nullptr;
		Material* material = nullptr;
	};
}