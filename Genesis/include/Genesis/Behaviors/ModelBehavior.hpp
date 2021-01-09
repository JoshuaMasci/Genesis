#pragma once

#include "Genesis/World/Behaviour.hpp"
#include "Genesis/Resource/Mesh.hpp"
#include "Genesis/Resource/Material.hpp"

namespace Genesis
{
	class ModelBehaviour
	{
	public:
		shared_ptr<Mesh> mesh;
		shared_ptr<Material> material;
	};
}