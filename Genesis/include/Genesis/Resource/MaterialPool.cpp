#include "MaterialPool.hpp"

#include "Genesis/Rendering/ResourceLoaders.hpp"

using namespace Genesis;

MaterialPool::MaterialPool(RenderingBackend* backend)
{
	this->backend = backend;

	{
		DescriptorSetLayoutCreateInfo layout_create_info = {};
		vector<DescriptorBinding> descriptor_binding =
		{
			{BindingType::Uniform_Buffer, (ShaderStage)ShaderStageBits::FRAGMENT}
		};

		for (uint32_t i = 0; i < Material::TextureCount; i++)
		{
			descriptor_binding.push_back({ BindingType::Combined_Image_Sampler, (ShaderStage)ShaderStageBits::FRAGMENT });
		}

		layout_create_info.descriptor_bindings = descriptor_binding.data();
		layout_create_info.descriptor_bindings_count = (uint32_t)descriptor_binding.size();

		this->material_layout = this->backend->createDescriptorSetLayout(layout_create_info);
	}

	{
		vector2U size(1, 1);
		vector<uint8_t> data(size.x * size.y * 4, 0);
		this->empty_texture = this->backend->createTexture(size, data.data(), sizeof(uint8_t) * data.size());
	}

	{
		SamplerCreateInfo info = {};
		this->material_sampler = this->backend->createSampler(info);
	}
}

MaterialPool::~MaterialPool()
{
	for (auto resource : this->resources)
	{
		this->unloadResource(resource.first);
	}

	this->backend->destroyTexture(this->empty_texture);
}

void MaterialPool::loadResource(string key)
{
	Material material = MaterialLoader::loadMaterial(this->backend, key);

	DescriptorSetCreateInfo descriptor_create_info;
	descriptor_create_info.layout = this->material_layout;

	vector<DescriptorSetBinding> bindings =
	{
		{ BindingType::Uniform_Buffer, nullptr, nullptr, material.values }
	};

	for (uint32_t i = 0; i < Material::TextureCount; i++)
	{
		Texture texture = material.textures[i];

		if (texture == nullptr)
		{
			texture = this->empty_texture;
		}
		
		bindings.push_back({ BindingType::Combined_Image_Sampler, this->material_sampler, texture, nullptr });
	}

	descriptor_create_info.descriptor_bindings = bindings.data();
	descriptor_create_info.descriptor_bindings_count = (uint32_t)bindings.size();

	material.material_descriptor = this->backend->createDescriptorSet(descriptor_create_info);

	this->resources[key].resource = material;
	this->resources[key].using_count = 0;
}

void MaterialPool::unloadResource(string key)
{
	ResourceInternal resource = this->resources[key];
	Material& material = resource.resource;

	this->backend->destroyStaticBuffer(material.values);

	for (size_t i = 0; i < Material::TextureCount; i++)
	{
		if (material.textures[i] != nullptr)
		{
			this->backend->destroyTexture(material.textures[i]);
		}
	}

	this->backend->destroyDescriptorSet(material.material_descriptor);
}
