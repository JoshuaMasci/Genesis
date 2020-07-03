#pragma once

#include "Genesis/Resource/PbrMesh.hpp"
#include "Genesis/Resource/PbrMaterial.hpp"

namespace Genesis
{
	struct MeshComponent
	{
		PbrMesh* mesh = nullptr;
	};

	struct WindowMeshComponent
	{
		enum class WindowView
		{
			ParentWorld,
			ChildWorld,
		};

		PbrMesh* mesh = nullptr;
		WindowView view;
	};
}