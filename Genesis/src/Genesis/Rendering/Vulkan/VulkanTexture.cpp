#include "VulkanTexture.hpp"

#include "Genesis/Rendering/Vulkan/VulkanImageUtils.hpp"

using namespace Genesis;

VulkanTexture::VulkanTexture(VulkanDevice* device, VulkanAllocator* allocator, VulkanDescriptorPool* descriptor_pool, VkExtent2D size, VmaMemoryUsage memory_usage, VkSampler sampler)
{
	this->device = device->get();
	this->allocator = allocator;
	this->descriptor_pool = descriptor_pool;
	this->size = size;

	VkImageCreateInfo image_info = {};
	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.imageType = VK_IMAGE_TYPE_2D;
	image_info.extent.width = this->size.width;
	image_info.extent.height = this->size.height;
	image_info.extent.depth = 1;
	image_info.mipLevels = 1;
	image_info.arrayLayers = 1;
	image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	image_info.format = VK_FORMAT_R8G8B8A8_UNORM;
	image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_info.samples = VK_SAMPLE_COUNT_1_BIT;
	image_info.flags = 0;
	this->allocator->createImage(&image_info, memory_usage, &this->image, &this->image_memory, &this->image_memory_info);

	VkImageViewCreateInfo view_info = {};
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.image = this->image;
	view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view_info.format = image_info.format;
	view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = 1;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = 1;
	vkCreateImageView(this->device, &view_info, nullptr, &this->image_view);

	this->image_descriptor_set = this->descriptor_pool->getDescriptorSet();
	VkDescriptorImageInfo descriptor_image_info = {};
	descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	descriptor_image_info.imageView = this->image_view;
	descriptor_image_info.sampler = sampler;
	VkWriteDescriptorSet descriptor_write = {};
	descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptor_write.dstSet = this->image_descriptor_set;
	descriptor_write.dstBinding = 0;
	descriptor_write.dstArrayElement = 0;
	descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptor_write.descriptorCount = 1;
	descriptor_write.pImageInfo = &descriptor_image_info;
	vkUpdateDescriptorSets(this->device, 1, &descriptor_write, 0, nullptr);
}

VulkanTexture::~VulkanTexture()
{
	this->descriptor_pool->freeDescriptorSet(this->image_descriptor_set);
	vkDestroyImageView(this->device, this->image_view, nullptr);
	this->allocator->destroyImage(this->image, this->image_memory);
}

void VulkanTexture::fillTexture(VulkanCommandPool* transfer_pool, VkQueue transfer_queue, void* data, uint64_t data_size)
{
	const uint32_t bytes_per_pixel = 4;
	if (data_size != this->size.width * this->size.height * bytes_per_pixel)
	{
		printf("Error: image not correct size for texture\n");
		return;
	}

	//Staging Buffer
	VkBuffer staging_buffer;
	VmaAllocation staging_buffer_memory;
	VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferInfo.size = data_size;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	this->allocator->createBuffer(&bufferInfo, VMA_MEMORY_USAGE_CPU_ONLY, &staging_buffer, &staging_buffer_memory, nullptr);

	void* mapped_data;
	this->allocator->mapMemory(staging_buffer_memory, &mapped_data);
	memcpy(mapped_data, data, data_size);
	this->allocator->unmapMemory(staging_buffer_memory);

	transitionImageLayout(transfer_pool, transfer_queue, this->image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(transfer_pool, transfer_queue, staging_buffer, this->image, this->size);
	transitionImageLayout(transfer_pool, transfer_queue, this->image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	this->allocator->destroyBuffer(staging_buffer, staging_buffer_memory);
}
