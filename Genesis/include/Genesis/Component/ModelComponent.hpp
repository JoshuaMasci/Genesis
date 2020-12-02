#pragma once

#include "Genesis/Resource/MeshResource.hpp"
#include "Genesis/Resource/Material.hpp"

namespace Genesis
{
	struct ModelComponent
	{
		shared_ptr<MeshResource> mesh;
		shared_ptr<Material> material;
	};
}