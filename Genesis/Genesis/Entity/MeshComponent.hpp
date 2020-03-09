#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Scene/Mesh.hpp"

namespace Genesis
{
	struct MeshComponent
	{
		string mesh_file;
		Mesh* mesh = nullptr;
	};
}