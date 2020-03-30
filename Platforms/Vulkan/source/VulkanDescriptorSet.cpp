#include "VulkanDescriptorSet.hpp"

#include "VulkanBuffer.hpp"
#include "VulkanTexture.hpp"

using namespace Genesis;

VulkanDescriptorSet::VulkanDescriptorSet(VkDevice device, VulkanDescriptorPool2* descriptor_pool, const DescriptorSetCreateInfo& create_info)
{
	this->descriptor_pool = descriptor_pool;

	this->descriptor_set = this->descriptor_pool->createDescriptorSet((VkDescriptorSetLayout)create_info.layout);

	size_t binding_count = create_info.descriptor_bindings_count;
	List<VkWriteDescriptorSet> descriptor_info(binding_count);
	List<VkDescriptorBufferInfo> buffer_info(binding_count);
	List<VkDescriptorImageInfo> image_sampler_info(binding_count);
	for (uint32_t i = 0; i < binding_count; i++)
	{
		if (create_info.descriptor_bindings[i].type == BindingType::Uniform_Buffer)
		{
			VulkanBuffer* buffer = (VulkanBuffer*)create_info.descriptor_bindings[i].buffer;

			buffer_info[i].buffer = buffer->get();
			buffer_info[i].offset = 0;
			buffer_info[i].range = (VkDeviceSize)buffer->getSize();

			descriptor_info[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptor_info[i].pNext = VK_NULL_HANDLE;
			descriptor_info[i].dstSet = this->descriptor_set;
			descriptor_info[i].dstBinding = (uint32_t)i;
			descriptor_info[i].dstArrayElement = 0;
			descriptor_info[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptor_info[i].descriptorCount = 1;
			descriptor_info[i].pBufferInfo = &buffer_info[i];
			descriptor_info[i].pImageInfo = nullptr;
			descriptor_info[i].pTexelBufferView = nullptr;
		}
		else if (create_info.descriptor_bindings[i].type == BindingType::Combined_Image_Sampler)
		{
			VulkanTexture* texture = (VulkanTexture*)create_info.descriptor_bindings[i].texture;

			image_sampler_info[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			image_sampler_info[i].imageView = texture->getImageView();
			image_sampler_info[i].sampler = (VkSampler)create_info.descriptor_bindings[i].sampler;

			descriptor_info[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptor_info[i].pNext = VK_NULL_HANDLE;
			descriptor_info[i].dstSet = this->descriptor_set;
			descriptor_info[i].dstBinding = (uint32_t)i;
			descriptor_info[i].dstArrayElement = 0;
			descriptor_info[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptor_info[i].descriptorCount = 1;
			descriptor_info[i].pBufferInfo = nullptr;
			descriptor_info[i].pImageInfo = &image_sampler_info[i];
			descriptor_info[i].pTexelBufferView = nullptr;
		}
	}
	vkUpdateDescriptorSets(device, (uint32_t)descriptor_info.size(), descriptor_info.data(), 0, nullptr);
}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
	this->descriptor_pool->destroyDescriptorSet(this->descriptor_set);
}
