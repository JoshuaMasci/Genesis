#pragma once

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/RenderingBackend/RenderingTypes.hpp"
#include "Genesis/RenderingBackend/RenderingBackend.hpp"

namespace Genesis
{
	struct MaterialValues
	{
		vector4F albedo;
		float metallic;
		float roughness;
		float ambient_occlusion;
		float height_scale = 0.0f;

		bool has_albedo_map = false;
		bool has_normal_map = false;
		bool has_height_map = false;
		bool has_metallic_map = false;
		bool has_roughness_map = false;
		bool has_ambient_occlusion_map = false;
	};

	struct MaterialSettings
	{
		bool transparent = false;
		bool unshaded = false;
		bool no_depth_test = false;
		bool no_shadows = false;
	};

	struct MaterialValuesPacked
	{
		vector4F albedo;
		vector4F pbr_values;
		vector4U has_textures1;
		vector2U has_textures2;
	};

	struct MaterialMaps
	{
		Texture albedo_map = nullptr;

		Texture normal_map = nullptr;
		Texture height_map = nullptr;

		Texture metallic_map = nullptr;
		Texture roughness_map = nullptr;
		Texture occlusion_map = nullptr;
	};

	struct MaterialBindings
	{
		StaticBuffer values = nullptr;
		Texture albedo_map = nullptr;
		Texture normal_map = nullptr;
		Texture height_map = nullptr;
		Texture metallic_map = nullptr;
		Texture roughness_map = nullptr;
		Texture occlusion_map = nullptr;
	};

	struct Material
	{
		static Material createMaterial(RenderingBackend* backend, const MaterialValues& values, Texture empty_texture)
		{

			MaterialValuesPacked packed;
			packed.albedo = values.albedo;
			packed.pbr_values = vector4F(values.metallic, values.roughness, values.ambient_occlusion, values.height_scale);
			packed.has_textures1 = vector4U(0);
			packed.has_textures2 = vector2U(0);

			Material material;

			material.bindings.values = backend->createStaticBuffer(&packed, sizeof(MaterialValuesPacked), BufferUsage::Uniform_Buffer, MemoryType::GPU_Only);
			material.bindings.albedo_map = empty_texture;
			material.bindings.normal_map = empty_texture;
			material.bindings.height_map = empty_texture;
			material.bindings.metallic_map = empty_texture;
			material.bindings.roughness_map = empty_texture;
			material.bindings.occlusion_map = empty_texture;

			return material;
		}

		//MaterialSettings settings;
		MaterialBindings bindings;
	};
}