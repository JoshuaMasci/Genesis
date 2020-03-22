#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Resource/Mesh.hpp"

namespace Genesis
{
	struct MeshComponent
	{
		MeshComponent(string file, Mesh* mesh)
		{
			this->mesh_file = file;
			this->mesh = mesh;
		};

		string mesh_file;
		Mesh* mesh;
	};
}