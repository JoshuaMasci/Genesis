#include "genesis_engine/resource/material_pool.hpp"

#include <fstream>
#include "genesis_engine/core/yaml.hpp"

namespace genesis
{
	MaterialPool::MaterialPool(TexturePool* texture_pool)
	{
		this->texture_pool = texture_pool;
	}

	shared_ptr<Material> MaterialPool::loadResource(const string& key)
	{
		YAML::Node material_node = YAML::LoadFile(key);

		shared_ptr<Material> material = std::make_shared<Material>(key);

		if (material_node["albedo_factor"])
		{
			material->albedo_factor = material_node["albedo_factor"].as<vec4f>();
		}

		if (material_node["metallic_roughness_factor"])
		{
			material->metallic_roughness_factor = material_node["metallic_roughness_factor"].as<vec2f>();
		}

		if (material_node["emissive_factor"])
		{
			material->emissive_factor = material_node["emissive_factor"].as<vec4f>();
		}

		if (material_node["albedo_texture"])
		{
			material->albedo_texture.texture = this->texture_pool->getResource(material_node["albedo_texture"].as<string>());
			material->albedo_texture.uv = 1;
		}

		if (material_node["normal_texture"])
		{
			material->normal_texture.texture = this->texture_pool->getResource(material_node["normal_texture"].as<string>());
			material->normal_texture.uv = 1;
		}

		if (material_node["metallic_roughness_texture"])
		{
			material->metallic_roughness_texture.texture = this->texture_pool->getResource(material_node["metallic_roughness_texture"].as<string>());
			material->metallic_roughness_texture.uv = 1;
		}

		if (material_node["occlusion_texture"])
		{
			material->occlusion_texture.texture = this->texture_pool->getResource(material_node["occlusion_texture"].as<string>());
			material->occlusion_texture.uv = 1;
		}

		if (material_node["emissive_texture"])
		{
			material->emissive_texture.texture = this->texture_pool->getResource(material_node["emissive_texture"].as<string>());
			material->emissive_texture.uv = 1;
		}

		if (material_node["cull_backface"])
		{
			material->cull_backface = material_node["cull_backface"].as<bool>();
		}

		if (material_node["transparent"])
		{
			material->transparent = material_node["transparent"].as<bool>();
		}

		return material;
	}
}