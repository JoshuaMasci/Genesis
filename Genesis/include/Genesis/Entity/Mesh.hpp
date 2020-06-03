#pragma once

#include "Genesis/Entity/Component.hpp"

#include "Genesis/Resource/PbrMesh.hpp"
#include "Genesis/Resource/PbrMaterial.hpp"

namespace Genesis
{
	class MeshComponent : public Component
	{
	public:
		MeshComponent(Node* node, PbrMesh* mesh);

		PbrMesh* mesh = nullptr;
	};
}