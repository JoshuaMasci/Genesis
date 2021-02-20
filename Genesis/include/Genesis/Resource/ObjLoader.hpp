#pragma once

#include "Genesis/Resource/Mesh.hpp"
#include "Genesis/LegacyBackend/LegacyBackend.hpp"

namespace Genesis
{
	struct ObjLoader
	{
		static MeshStruct loadMesh(LegacyBackend* backend, const string& filename);
	};
}