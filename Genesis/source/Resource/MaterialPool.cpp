#include "Genesis/Resource/MaterialPool.hpp"

#include <jsoncons/json.hpp>
#include <fstream>

namespace Genesis
{
	using namespace jsoncons;

	MaterialPool::MaterialPool(TexturePool* texture_pool)
	{
		this->texture_pool = texture_pool;
	}

	/*MaterialTexture albedo_texture;
		MaterialTexture normal_texture;
		MaterialTexture metallic_roughness_texture;
		MaterialTexture occlusion_texture;
		MaterialTexture emissive_texture;*/

	shared_ptr<Material> MaterialPool::loadResource(const string& key)
	{
		std::ifstream in_stream(key);

		if (!in_stream.is_open())
		{
			GENESIS_ENGINE_ASSERT("Failed to open Material File {}", key);
			return nullptr;
		}

		jsoncons::json material_file = json::parse(in_stream);
		shared_ptr<Material> material = std::make_shared<Material>(key);

		if (material_file.contains("albedo_factor"))
		{
			vector<float> factor = material_file["albedo_factor"].as<vector<float>>();
			material->albedo_factor = vector4F(factor[0], factor[1], factor[2], factor[3]);
		}

		if (material_file.contains("metallic_roughness_factor"))
		{
			vector<float> factor = material_file["metallic_roughness_factor"].as<vector<float>>();
			material->metallic_roughness_factor = vector2F(factor[0], factor[1]);
		}

		if (material_file.contains("emissive_factor"))
		{
			vector<float> factor = material_file["emissive_factor"].as<vector<float>>();
			material->emissive_factor = vector4F(factor[0], factor[1], factor[2], factor[3]);
		}

		if (material_file.contains("albedo_texture"))
		{
			material->albedo_texture.texture = this->texture_pool->getResource(material_file.get("albedo_texture").as_string());
			material->albedo_texture.uv = 1;
		}

		if (material_file.contains("normal_texture"))
		{
			material->normal_texture.texture = this->texture_pool->getResource(material_file.get("normal_texture").as_string());
			material->normal_texture.uv = 1;
		}

		if (material_file.contains("metallic_roughness_texture"))
		{
			material->metallic_roughness_texture.texture = this->texture_pool->getResource(material_file.get("metallic_roughness_texture").as_string());
			material->metallic_roughness_texture.uv = 1;
		}

		if (material_file.contains("occlusion_texture"))
		{
			material->occlusion_texture.texture = this->texture_pool->getResource(material_file.get("occlusion_texture").as_string());
			material->occlusion_texture.uv = 1;
		}

		if (material_file.contains("emissive_texture"))
		{
			material->emissive_texture.texture = this->texture_pool->getResource(material_file.get("emissive_texture").as_string());
			material->emissive_texture.uv = 1;
		}

		if (material_file.contains("cull_backface"))
		{
			material->cull_backface = material_file["cull_backface"].as_bool();
		}

		if (material_file.contains("transparent"))
		{
			material->transparent = material_file["transparent"].as_bool();
		}

		return material;
	}
}