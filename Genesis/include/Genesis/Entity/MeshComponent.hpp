#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Scene/Mesh.hpp"

namespace Genesis
{
	struct MeshComponent
	{
		MeshComponent(string file, MeshPool* pool)
		{
			this->mesh_file = file;
			this->mesh_pool = pool;

			this->mesh = this->mesh_pool->getResource(this->mesh_file);
		};

		~MeshComponent()
		{
			this->mesh_pool->freeResource(this->mesh_file);
		};

		string mesh_file;
		MeshPool* mesh_pool;
		Mesh* mesh;
	};
}