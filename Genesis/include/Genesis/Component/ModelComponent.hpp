#pragma once

#include "Genesis/Resource/MeshResource.hpp"
#include "Genesis/Resource/Material.hpp"

namespace Genesis
{
	struct ModelComponent
	{
		shared_ptr<MeshResource> mesh;
		shared_ptr<Material> material;

		//Stores a copy of the world transform for faster rendering
		//only a copy, changes to this do not propagate
		TransformD world_transform;
	};
}