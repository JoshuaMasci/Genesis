#pragma once

#include "Genesis/Resource/MaterialPool.hpp"
#include "Genesis/Resource/TexturePool.hpp"
#include "Genesis/Resource/Material.hpp"

namespace Genesis
{
	class MaterialEditorWindow
	{
	protected:
		shared_ptr<Material> active_material = nullptr;

		MaterialPool* material_pool = nullptr;
		TexturePool* texture_pool = nullptr;
		shared_ptr<TextureResource> grid_texture;

	public:
		MaterialEditorWindow(MaterialPool* material_pool, TexturePool* texture_pool);
		~MaterialEditorWindow();

		void setActiveMaterial(shared_ptr<Material> material);
		void drawTexture(Material::MaterialTexture& texture_slot);
		void draw();
	};
}