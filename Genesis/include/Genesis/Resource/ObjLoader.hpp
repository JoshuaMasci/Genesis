#pragma once

#include "Genesis/Resource/Mesh.hpp"
#include "Genesis/LegacyBackend/LegacyBackend.hpp"

namespace Genesis
{
	struct ObjLoader
	{
		static Mesh loadMesh(LegacyBackend* backend, const string& filename);
	};
}