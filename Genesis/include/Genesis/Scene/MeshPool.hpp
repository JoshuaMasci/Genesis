#pragma once

#include "Genesis/Scene/Mesh.hpp"
#include "Genesis/Scene/ResourcePool.hpp"
#include "Genesis/Rendering/RenderingBackend.hpp"

namespace Genesis
{
	class MeshPool : public ResourcePool<string, Mesh>
	{
	public:
		MeshPool(RenderingBackend* backend);

	protected:
		virtual void loadResource(string key) override;
		virtual void unloadResource(string key) override;

		RenderingBackend* backend;
		VertexInputDescription vertex_input;
	};
}