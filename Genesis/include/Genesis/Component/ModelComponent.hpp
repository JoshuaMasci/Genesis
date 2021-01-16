#pragma once

#include "Genesis/Resource/Mesh.hpp"
#include "Genesis/Resource/Material.hpp"

namespace Genesis
{
	struct ModelComponent
	{
		shared_ptr<Mesh> mesh;
		shared_ptr<Material> material;
	};
}