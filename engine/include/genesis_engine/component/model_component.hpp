#pragma once

#include "genesis_engine/resource/Mesh.hpp"
#include "genesis_engine/resource/Material.hpp"

namespace genesis
{
	struct ModelComponent
	{
		shared_ptr<Mesh> mesh;
		shared_ptr<Material> material;
	};
}