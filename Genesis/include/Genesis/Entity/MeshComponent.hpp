#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Resource/Mesh.hpp"
#include "Genesis/Resource/Material.hpp"

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

	struct MaterialMeshComponent
	{
		MaterialMeshComponent(void* mesh, void* material)
		{
			this->mesh = mesh;
			this->material = material;
		};
		void* mesh = nullptr;
		void* material = nullptr;
	};
}