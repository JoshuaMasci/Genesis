#pragma once

#include "Genesis/Resource/Texture.hpp"
#include "Genesis/Resource/Material.hpp"

namespace Genesis
{
	class ResourceManager;

	class MaterialEditorWindow
	{
	public:
		MaterialEditorWindow(ResourceManager* resource_manager);

		void setActiveMaterial(shared_ptr<Material> material);
		void draw();

	protected:
		void drawTexture(Material::MaterialTexture& texture_slot);

		shared_ptr<Material> active_material = nullptr;

		ResourceManager* resource_manager = nullptr;
		shared_ptr<Texture> grid_texture;
	};
}