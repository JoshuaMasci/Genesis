#include "VulkanModel.hpp"

Genesis::VulkanModel::VulkanModel(vector<Vertex> vertices, vector<uint32_t> indices, VmaAllocator& allocator)
{
	this->allocator = &allocator;

	//Vertex
	VkBufferCreateInfo vertex_buffer_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	vertex_buffer_info.size = sizeof(Vertex) * vertices.size();
	vertex_buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	VmaAllocationCreateInfo vertex_alloc_info = {};
	vertex_alloc_info.usage = VMA_MEMORY_USAGE_CPU_ONLY;
	vmaCreateBuffer(allocator, &vertex_buffer_info, &vertex_alloc_info, &this->vertex_buffer, &this->vertex_buffer_memory, nullptr);

	//FILL THE BUFFER!!!!!
	void* vertex_data;
	vmaMapMemory(allocator, this->vertex_buffer_memory, &vertex_data);
	memcpy(vertex_data, vertices.data(), vertex_buffer_info.size);
	vmaUnmapMemory(allocator, this->vertex_buffer_memory);


	//Vertex
	VkBufferCreateInfo index_buffer_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	index_buffer_info.size = sizeof(uint32_t) * indices.size();
	index_buffer_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	VmaAllocationCreateInfo index_alloc_info = {};
	index_alloc_info.usage = VMA_MEMORY_USAGE_CPU_ONLY;
	vmaCreateBuffer(allocator, &index_buffer_info, &index_alloc_info, &this->index_buffer, &this->index_buffer_memory, nullptr);

	//FILL THE BUFFER!!!!!
	void* index_data;
	vmaMapMemory(allocator, this->index_buffer_memory, &index_data);
	memcpy(index_data, indices.data(), index_buffer_info.size);
	vmaUnmapMemory(allocator, this->index_buffer_memory);

	this->number_of_indices = indices.size();
}

Genesis::VulkanModel::~VulkanModel()
{
	vmaDestroyBuffer(*this->allocator, this->vertex_buffer, this->vertex_buffer_memory);
	vmaDestroyBuffer(*this->allocator, this->index_buffer, this->index_buffer_memory);
}
