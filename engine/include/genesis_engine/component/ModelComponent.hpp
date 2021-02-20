#pragma once

#include "genesis_engine/resource/Mesh.hpp"
#include "genesis_engine/resource/Material.hpp"

namespace genesis_engine
{
	struct ModelComponent
	{
		shared_ptr<Mesh> mesh;
		shared_ptr<Material> material;
	};
}