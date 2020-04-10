#include "Genesis/LegacyRendering/LegacyMaterialPool.hpp"

#include "Genesis/Resource/ResourceLoaders.hpp"

using namespace Genesis;

LegacyMaterialPool::LegacyMaterialPool(LegacyBackend* backend, LegacyTexturePool* pool)
{
	this->legacy_backend = backend;
	this->texture_pool = pool;
}

LegacyMaterialPool::~LegacyMaterialPool()
{
	for (auto resource : this->resources)
	{
		this->unloadResource(resource.first);
	}
}

void LegacyMaterialPool::loadResource(string key)
{
	MaterialDescription material_description = MaterialLoader::loadMaterial(key);

	LegacyMaterial material = {};
	material.albedo = material_description.albedo;
	material.pbr_values = vector4F(material_description.metallic, material_description.roughness, material_description.ambient_occlusion, material_description.height_scale);

	for (size_t i = 0; i < LegacyMaterial::TextureCount; i++)
	{
		material.texture_names[i] = material_description.textures[i];

		if (!material.texture_names[i].empty())
		{
			material.textures[i] = *this->texture_pool->getResource(material.texture_names[i]);
		}
	}

	this->resources[key].resource = material;
	this->resources[key].using_count = 0;
}

void LegacyMaterialPool::unloadResource(string key)
{
	LegacyMaterial& material = this->resources[key].resource;

	for (size_t i = 0; i < LegacyMaterial::TextureCount; i++)
	{
		if (!material.texture_names[i].empty())
		{
			this->texture_pool->freeResource(material.texture_names[i]);
		}
	}
}
