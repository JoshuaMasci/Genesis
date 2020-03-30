#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Resource/Mesh.hpp"

namespace Genesis
{
	struct MeshComponent
	{
		MeshComponent(Mesh* mesh, Material* material)
		{
			this->mesh = mesh;
			this->material = material;
		};
		Mesh* mesh;
		Material* material;
	};
}