#pragma once

#include "Genesis/Resource/Mesh.hpp"
#include "Genesis/Resource/ResourcePool.hpp"
#include "Genesis/RenderingBackend/RenderingBackend.hpp"

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