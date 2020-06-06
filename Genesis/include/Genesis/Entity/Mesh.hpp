#pragma once

#include "Genesis/Entity/Component.hpp"

#include "Genesis/Resource/PbrMesh.hpp"
#include "Genesis/Resource/PbrMaterial.hpp"

namespace Genesis
{
	class MeshComponent : public Component
	{
	public:
		MeshComponent(Node* node, PbrMesh* mesh)
		: Component(node)
		{
			this->mesh = mesh;
		}

		PbrMesh* mesh = nullptr;
	};

	class WindowMeshComponent : public Component
	{
	public:

		enum class WindowView
		{
			ParentWorld,
			ChildWorld,
		};

		WindowMeshComponent(Node* node, PbrMesh* mesh, WindowView view)
			: Component(node)
		{
			this->mesh = mesh;
			this->view = view;
		}

		PbrMesh* mesh = nullptr;
		WindowView view;
	};
}