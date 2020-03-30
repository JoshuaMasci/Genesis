#pragma once

#include "Genesis/Resource/Material.hpp"
#include "Genesis/Resource/ResourcePool.hpp"
#include "Genesis/RenderingBackend/RenderingBackend.hpp"

namespace Genesis
{
	class MaterialPool : public ResourcePool<string, Material>
	{
	public:
		MaterialPool(RenderingBackend* backend);
		virtual ~MaterialPool();

		inline DescriptorSetLayout getMaterialLayout() { return this->material_layout; };

	protected:
		virtual void loadResource(string key) override;
		virtual void unloadResource(string key) override;

		RenderingBackend* backend;

		Texture empty_texture;
		Sampler material_sampler;
		DescriptorSetLayout material_layout;
	};
}