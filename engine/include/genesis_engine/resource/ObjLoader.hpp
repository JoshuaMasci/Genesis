#pragma once

#include "genesis_engine/resource/Mesh.hpp"
#include "genesis_engine/LegacyBackend/LegacyBackend.hpp"

namespace genesis_engine
{
	struct ObjLoader
	{
		static MeshStruct loadMesh(LegacyBackend* backend, const string& filename);
	};
}