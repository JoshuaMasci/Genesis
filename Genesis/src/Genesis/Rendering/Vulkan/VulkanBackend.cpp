#include "VulkanBackend.hpp"

#define VMA_IMPLEMENTATION

#include "Genesis/Rendering/Vulkan/VulkanInstance.hpp"
#include "Genesis/Rendering/Vulkan/VulkanBuffer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanRenderPipline.hpp"

using namespace Genesis;

VkBufferUsageFlags getBufferUsage(BufferType usage)
{
	switch (usage)
	{
	case BufferType::Uniform:
		return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; //Probobly won't be held in GPU_Only space, copy bit not needed for now
	case BufferType::Index:
		return VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT; //Likely going to need to copy into the buffer
	case BufferType::Vertex:
		return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT; //Likely going to need to copy into the buffer
	}
	return 0;
}

VmaMemoryUsage getMemoryUsage(MemoryUsage memory_usage)
{
	switch (memory_usage)
	{
	case MemoryUsage::GPU_Only:
		return VMA_MEMORY_USAGE_GPU_ONLY;
	case MemoryUsage::CPU_Visable:
		return VMA_MEMORY_USAGE_CPU_TO_GPU;//Subject To Change
	}
	return VMA_MEMORY_USAGE_UNKNOWN;
}

VulkanBackend::VulkanBackend(Window* window, uint32_t number_of_threads)
{
	this->vulkan = new VulkanInstance(window, number_of_threads);

	//Cube
	std::vector<Vertex> vertices =
	{
	{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
	{{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
	{{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
	{{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
	{{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
	{{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}}
	};
	this->cube_vertices = this->createBuffer(sizeof(Vertex) * vertices.size(), BufferType::Vertex, MemoryUsage::GPU_Only);
	this->cube_vertices->fillBuffer(vertices.data(), sizeof(Vertex) * vertices.size());

	std::vector<uint16_t> indices =
	{
	5, 6, 4, 4, 6, 7, //front
	1, 0, 2, 2, 0, 3, //back
	1, 2, 5, 5, 2, 6, //left
	0, 4, 3, 3, 4, 7, //right
	2, 3, 6, 6, 3, 7, //top
	0, 1, 4, 4, 1, 5 //bottom
	};
	this->cube_indices = this->createBuffer(sizeof(uint16_t) * indices.size(), BufferType::Index, MemoryUsage::GPU_Only);
	this->cube_indices->fillBuffer(indices.data(), sizeof(uint16_t) * indices.size());
	this->indices_count = (uint32_t)indices.size();
}

VulkanBackend::~VulkanBackend()
{
	delete this->vulkan;
}

void VulkanBackend::beginFrame()
{
	vkWaitForFences(this->vulkan->device->getDevice(), 1, &this->vulkan->in_flight_fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
	vkResetFences(this->vulkan->device->getDevice(), 1, &this->vulkan->in_flight_fence);

	VkResult result = vkAcquireNextImageKHR(this->vulkan->device->getDevice(), this->vulkan->swapchain->getSwapchain(), std::numeric_limits<uint64_t>::max(), this->vulkan->image_available_semaphore, VK_NULL_HANDLE, &this->swapchain_image_index);

	//TEMP STUFF
	VkCommandBuffer buffer = this->vulkan->command_pool->getGraphicsBuffer(0);
	vkResetCommandBuffer(buffer, 0);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	if (vkBeginCommandBuffer(buffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkExtent2D swapchain_extent = this->vulkan->swapchain->getSwapchainExtent();

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = this->vulkan->screen_render_pass;
	renderPassInfo.framebuffer = this->vulkan->swapchain->getSwapchainFramebuffer(this->swapchain_image_index);
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapchain_extent;

	VkClearValue clearValues[2];
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = 2;
	renderPassInfo.pClearValues = clearValues;

	vkCmdBeginRenderPass(buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	matrix4F mvp;
	if (true)
	{
		float fov = 1.0f / tan(glm::radians(75.0f) / 2.0f);
		float aspect = (float)swapchain_extent.width / (float)swapchain_extent.height;

		const float turn_rate = (float)PI / 2.0f;
		static float angle = 0.0f;
		quaternionF quat = glm::angleAxis(angle, vector3F(0.0f, 1.0f, 0.0f));
		angle += (turn_rate * (1.0f/2000.0f));

		matrix4F model = glm::toMat4(quat);
		matrix4F view = glm::lookAt(vector3F(0.0f, 0.0f, -2.0f), vector3F(0.0f), vector3F(0.0f, 1.0f, 0.0f));
		matrix4F proj = glm::infinitePerspective(fov, aspect, 0.1f);
		proj[1][1] *= -1; //Need to apply this because vulkan flips the y-axis and I don't like that

		mvp = proj * view * model;
	}

	vkCmdPushConstants(buffer, this->vulkan->colored_mesh_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(matrix4F), &mvp);//TODO
	vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->vulkan->colored_mesh_screen_pipeline->getPipeline());

	VkBuffer vertexBuffers[] = { ((VulkanBuffer*)this->cube_vertices)->getBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(buffer, ((VulkanBuffer*)this->cube_indices)->getBuffer(), 0, VK_INDEX_TYPE_UINT16);

	vkCmdDrawIndexed(buffer, this->indices_count, 1, 0, 0, 0);

	vkCmdEndRenderPass(buffer);

	if (vkEndCommandBuffer(buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

void VulkanBackend::endFrame()
{
	//Submit CommandBuffers
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { this->vulkan->image_available_semaphore};
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	//Only Submit one for now
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = this->vulkan->command_pool->getGraphicsBufferArray();

	VkSemaphore signalSemaphores[] = { this->vulkan->render_finished_semaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(this->vulkan->device->getGraphicsQueue(), 1, &submitInfo, this->vulkan->in_flight_fence) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	//Present the image to the screen
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { this->vulkan->swapchain->getSwapchain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &this->swapchain_image_index;

	VkResult result = vkQueuePresentKHR(this->vulkan->device->getPresentQueue(), &presentInfo);
}

Buffer* VulkanBackend::createBuffer(uint64_t size_bytes, BufferType type, MemoryUsage memory_usage)
{
	return new VulkanBuffer(this->vulkan, size_bytes, getBufferUsage(type), getMemoryUsage(memory_usage));
}
