#define VMA_IMPLEMENTATION

#include "VulkanRenderer.hpp"

using namespace Genesis;

#include "Genesis/Graphics/VulkanPhysicalDevicePicker.hpp"
#include "Genesis/Graphics/VulkanQueueFamilyIndices.hpp"
#include "Genesis/Graphics/VulkanSwapChainSupportDetails.hpp"
#include "Genesis/Graphics/VulkanShader.hpp"

#include <algorithm>

VulkanRenderer::VulkanRenderer(Window* window, const char* app_name)
{
	this->window = window;

	this->create_instance(app_name, VK_MAKE_VERSION(0, 0, 1));

	if (this->use_debug_layers)
	{
		this->create_debug_messenger();
	}

	this->create_surface();

	this->create_device_and_queues(VulkanPhysicalDevicePicker::pickDevice(this->context.instance, this->context.surface));

	this->create_allocator();

	this->create_swapchain();

	this->create_screen_render_pass();

	this->create_swapchain_framebuffers();

	this->create_descriptor_set_layout();

	this->create_pipeline();

	this->create_uniform_buffer();

	this->create_descriptor_set();

	this->create_cube_buffer();

	this->create_command_buffers();

	this->create_sync_objects();
}

VulkanRenderer::~VulkanRenderer()
{
	vkDeviceWaitIdle(this->context.device);

	this->delete_sync_objects();

	this->delete_command_buffers();

	this->delete_cube_buffer();

	this->delete_descriptor_set();

	this->delete_uniform_buffer();

	this->delete_pipeline();

	this->delete_descriptor_set_layout();

	this->delete_swapchain_framebuffers();

	this->delete_screen_render_pass();

	this->delete_swapchain();

	this->delete_allocator();

	this->delete_device_and_queues();

	this->delete_surface();

	if (this->use_debug_layers)
	{
		this->delete_debug_messenger();
	}

	this->delete_instance();
}

void VulkanRenderer::render(double delta_time)
{
	static uint32_t current_frame = 0;

	vkWaitForFences(this->context.device, 1, &this->context.in_flight_fences[current_frame], VK_TRUE, std::numeric_limits<uint64_t>::max());

	uint32_t image_index;
	VkResult result = vkAcquireNextImageKHR(this->context.device, this->context.swapchain, std::numeric_limits<uint64_t>::max(), this->context.image_available_semaphores[current_frame], VK_NULL_HANDLE, &image_index);
	
	if (result == VK_ERROR_OUT_OF_DATE_KHR) 
	{
		//throw std::runtime_error("Need to rebuild swapchain!!!");
		vkDeviceWaitIdle(this->context.device);

		this->delete_command_buffers();
		this->delete_descriptor_set();
		this->delete_pipeline();
		this->delete_swapchain_framebuffers();
		this->delete_screen_render_pass();
		this->delete_swapchain();

		this->create_swapchain();
		this->create_screen_render_pass();
		this->create_swapchain_framebuffers();
		this->create_pipeline();
		this->create_descriptor_set();
		this->create_command_buffers();

		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) 
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	float fov = 1.0f / tan(glm::radians(75.0f) / 2.0f);
	float aspect = (float)this->context.swapchain_properties.swapchain_extent.width / (float)this->context.swapchain_properties.swapchain_extent.height;

	const float turn_rate = (float)PI / 2.0f;
	static float angle = 0.0f;
	quaternionF quat = glm::angleAxis(angle, vector3F(0.0f, 1.0f, 0.0f));
	angle += (turn_rate * (float)delta_time);

	matrix4F model = glm::toMat4(quat);
	matrix4F view = glm::lookAt(vector3F(0.0f, 0.0f, -2.0f), vector3F(0.0f), vector3F(0.0f, 1.0f, 0.0f));
	matrix4F proj = glm::infinitePerspective(fov, aspect, 0.1f);
	proj[1][1] *= -1; //Need to apply this because vulkan flips the y-axis and I don't like that

	matrix4F matrix = proj * view * model;

	this->update_uniform_buffer(image_index, matrix);
	this->buildCommandBuffer(image_index);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { this->context.image_available_semaphores[current_frame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &this->context.command_buffers[image_index];

	VkSemaphore signalSemaphores[] = { this->context.render_finished_semaphores[current_frame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(this->context.device, 1, &this->context.in_flight_fences[current_frame]);

	if (vkQueueSubmit(this->context.graphics_queue, 1, &submitInfo, this->context.in_flight_fences[current_frame]) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { this->context.swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &image_index;

	result = vkQueuePresentKHR(this->context.present_queue, &presentInfo);

	current_frame = (current_frame + 1) % this->context.swapchain_image_count;
}

void Genesis::VulkanRenderer::create_pipeline()
{
	VkExtent2D swapChainExtent = this->context.swapchain_properties.swapchain_extent;

	VulkanShader shader = VulkanShader(this->context.device, "shader/vulkan/vert.spv", "shader/vulkan/frag.spv");

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChainExtent.width;
	viewport.height = (float)swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &this->descriptor_set_layout;

	if (vkCreatePipelineLayout(this->context.device, &pipelineLayoutInfo, nullptr, &this->pipeline_layout) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}

	vector<VkPipelineShaderStageCreateInfo> shaderStages = shader.getShaderStages();

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = (uint32_t)shaderStages.size();
	pipelineInfo.pStages = shaderStages.data();
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = this->pipeline_layout;
	pipelineInfo.renderPass = this->context.screen_render_pass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(this->context.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &this->pipeline) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}
}

void Genesis::VulkanRenderer::delete_pipeline()
{
	vkDestroyPipeline(this->context.device, this->pipeline, nullptr);
	vkDestroyPipelineLayout(this->context.device, this->pipeline_layout, nullptr);
}

void Genesis::VulkanRenderer::create_descriptor_set_layout()
{
	VkDescriptorSetLayoutBinding layout_binding = {};
	layout_binding.binding = 0;
	layout_binding.descriptorCount = 1;
	layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layout_binding.pImmutableSamplers = nullptr;
	layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &layout_binding;

	if (vkCreateDescriptorSetLayout(this->context.device, &layoutInfo, nullptr, &this->descriptor_set_layout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void Genesis::VulkanRenderer::delete_descriptor_set_layout()
{
	vkDestroyDescriptorSetLayout(this->context.device, this->descriptor_set_layout, nullptr);
}


void createBuffer(VmaAllocator& allocator, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memory_type, VkBuffer& buffer, VmaAllocation& buffer_memory)
{
	VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferInfo.size = size;
	bufferInfo.usage = usage;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = memory_type;

	vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &buffer_memory, nullptr);
}

void Genesis::VulkanRenderer::create_uniform_buffer()
{
	VkDeviceSize bufferSize = sizeof(UniformMatrices);

	this->uniform_buffers.resize(this->context.swapchain_image_count);
	this->uniform_buffers_memory.resize(this->context.swapchain_image_count);

	for (size_t i = 0; i < this->uniform_buffers.size(); i++)
	{
		createBuffer(this->context.allocator, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_ONLY, this->uniform_buffers[i], this->uniform_buffers_memory[i]);
	}
}

void Genesis::VulkanRenderer::delete_uniform_buffer()
{
	for (size_t i = 0; i < this->uniform_buffers.size(); i++)
	{
		vmaDestroyBuffer(this->context.allocator, this->uniform_buffers[i], this->uniform_buffers_memory[i]);
	}
}

void Genesis::VulkanRenderer::update_uniform_buffer(uint32_t index, matrix4F mvp)
{
	UniformMatrices matrix;
	//TODO some updating
	matrix.ModelViewProj = mvp;

	void* data;
	vmaMapMemory(this->context.allocator, this->uniform_buffers_memory[index], &data);
	memcpy(data, &matrix, sizeof(UniformMatrices));
	vmaUnmapMemory(this->context.allocator, this->uniform_buffers_memory[index]);
}

void Genesis::VulkanRenderer::create_descriptor_set()
{
	VkDescriptorPoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = this->context.swapchain_image_count;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = this->context.swapchain_image_count;

	if (vkCreateDescriptorPool(this->context.device, &poolInfo, nullptr, &descriptor_pool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}

	std::vector<VkDescriptorSetLayout> layouts(this->context.swapchain_image_count, this->descriptor_set_layout);
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptor_pool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
	allocInfo.pSetLayouts = layouts.data();

	descriptor_sets.resize(this->context.swapchain_image_count);
	if (vkAllocateDescriptorSets(this->context.device, &allocInfo, descriptor_sets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < descriptor_sets.size(); i++)
	{
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = this->uniform_buffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformMatrices);

		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptor_sets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(this->context.device, 1, &descriptorWrite, 0, nullptr);
	}
}

void Genesis::VulkanRenderer::delete_descriptor_set()
{
	vkDestroyDescriptorPool(this->context.device, this->descriptor_pool, nullptr);
}

uint32_t TEMP_SIZE;

void Genesis::VulkanRenderer::create_cube_buffer()
{
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

	std::vector<uint16_t> indices =
	{
	5, 6, 4, 4, 6, 7, //front
	1, 0, 2, 2, 0, 3, //back
	1, 2, 5, 5, 2, 6, //left
	0, 4, 3, 3, 4, 7, //right
	2, 3, 6, 6, 3, 7, //top
	0, 1, 4, 4, 1, 5 //bottom
	};

	TEMP_SIZE = (uint32_t)indices.size();

	VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();
	createBuffer(this->context.allocator, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_ONLY, this->vertex_buffer, this->vertex_buffer_memory);

	//FILL THE BUFFER!!!!!
	void* data;
	vmaMapMemory(this->context.allocator, this->vertex_buffer_memory, &data);
	memcpy(data, vertices.data(), bufferSize);
	vmaUnmapMemory(this->context.allocator, this->vertex_buffer_memory);

	bufferSize = sizeof(uint16_t) * indices.size();
	createBuffer(this->context.allocator, bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_ONLY, this->index_buffer, this->index_buffer_memory);

	//FILL THE BUFFER!!!!!
	vmaMapMemory(this->context.allocator, this->index_buffer_memory, &data);
	memcpy(data, indices.data(), bufferSize);
	vmaUnmapMemory(this->context.allocator, this->index_buffer_memory);
}

void Genesis::VulkanRenderer::delete_cube_buffer()
{
	vmaDestroyBuffer(this->context.allocator, this->vertex_buffer, this->vertex_buffer_memory);
	vmaDestroyBuffer(this->context.allocator, this->index_buffer, this->index_buffer_memory);
}

void Genesis::VulkanRenderer::buildCommandBuffer(uint32_t index)
{
	vkResetCommandBuffer(this->context.command_buffers[index], 0);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

	if (vkBeginCommandBuffer(this->context.command_buffers[index], &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = this->context.screen_render_pass;
	renderPassInfo.framebuffer = this->context.swapchain_images.swapchain_framebuffers[index];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = this->context.swapchain_properties.swapchain_extent;

	VkClearValue clearValues[2];
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = 2;
	renderPassInfo.pClearValues = clearValues;

	vkCmdBeginRenderPass(this->context.command_buffers[index], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindDescriptorSets(this->context.command_buffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipeline_layout, 0, 1, &this->descriptor_sets[index], 0, NULL);
	vkCmdBindPipeline(this->context.command_buffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipeline);

	VkBuffer vertexBuffers[] = { this->vertex_buffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(this->context.command_buffers[index], 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(this->context.command_buffers[index], this->index_buffer, 0, VK_INDEX_TYPE_UINT16);

	vkCmdDrawIndexed(this->context.command_buffers[index], TEMP_SIZE, 1, 0, 0, 0);

	vkCmdEndRenderPass(this->context.command_buffers[index]);

	if (vkEndCommandBuffer(this->context.command_buffers[index]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

void VulkanRenderer::create_instance(const char* app_name, uint32_t app_version)
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = app_name;
	appInfo.applicationVersion = app_version;
	appInfo.pEngineName = "Genesis";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &appInfo;

	vector<const char*> extensions = this->getExtensions();
	if (extensions.size() > 0)
	{
		create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		create_info.ppEnabledExtensionNames = extensions.data();
	}
	else
	{
		create_info.enabledExtensionCount = 0;
	}

	vector<const char*> layers = this->getLayers();
	if (layers.size() > 0)
	{
		create_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
		create_info.ppEnabledLayerNames = layers.data();
	}
	else
	{
		create_info.enabledLayerCount = 0;
	}

	VkResult result = vkCreateInstance(&create_info, nullptr, &this->context.instance);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create Vulkan Instance!");
	}
}

void VulkanRenderer::delete_instance()
{
	vkDestroyInstance(this->context.instance, nullptr);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	printf("Vulkan Validation Layer: %s\n", pCallbackData->pMessage);
	return VK_FALSE;
}

void VulkanRenderer::create_debug_messenger()
{
	VkDebugUtilsMessengerCreateInfoEXT create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	create_info.pfnUserCallback = debugCallback;

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(this->context.instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		VkResult result = func(this->context.instance, &create_info, nullptr, &this->context.debug_messenger);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}
	else
	{
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

void VulkanRenderer::delete_debug_messenger()
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(this->context.instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		func(this->context.instance, this->context.debug_messenger, nullptr);
	}
}

void VulkanRenderer::create_surface()
{
#ifdef VK_USE_PLATFORM_WIN32_KHR
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };

	surfaceCreateInfo.hinstance = GetModuleHandle(0);
	surfaceCreateInfo.hwnd = (HWND)this->window->getNativeWindowHandle();

	VkResult result = vkCreateWin32SurfaceKHR(this->context.instance, &surfaceCreateInfo, NULL, &this->context.surface);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to attach to surface");
	}
#endif // DEBUG
}

void VulkanRenderer::delete_surface()
{
	vkDestroySurfaceKHR(this->context.instance, this->context.surface, nullptr);
}

void VulkanRenderer::create_device_and_queues(VkPhysicalDevice physical_device)
{
	this->context.device_properties.physical_device = physical_device;
	vkGetPhysicalDeviceProperties(this->context.device_properties.physical_device, &this->context.device_properties.properties);
	vkGetPhysicalDeviceMemoryProperties(this->context.device_properties.physical_device, &this->context.device_properties.memory_properties);
	vkGetPhysicalDeviceFeatures(this->context.device_properties.physical_device, &this->context.device_properties.features);
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->context.device_properties.physical_device, this->context.surface, &this->context.device_properties.surface_capabilities);

	VulkanQueueFamilyIndices indices = VulkanQueueFamilyIndices::findQueueFamilies(this->context.device_properties.physical_device, this->context.surface);
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures device_features = {};
	//No device features needed as of now
	//TODO add as needed

	VkDeviceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	create_info.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	create_info.pQueueCreateInfos = queueCreateInfos.data();

	create_info.pEnabledFeatures = &device_features;

	create_info.enabledExtensionCount = 0;

	vector<const char*> extensions = this->getDeviceExtensions();
	if (extensions.size() > 0)
	{
		create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		create_info.ppEnabledExtensionNames = extensions.data();
	}
	else
	{
		create_info.enabledExtensionCount = 0;
	}

	vector<const char*> layers = this->getLayers();
	if (layers.size() > 0)
	{
		create_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
		create_info.ppEnabledLayerNames = layers.data();
	}
	else
	{
		create_info.enabledLayerCount = 0;
	}

	if (vkCreateDevice(this->context.device_properties.physical_device, &create_info, nullptr, &this->context.device) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(this->context.device, indices.graphicsFamily.value(), 0, &this->context.graphics_queue);
	vkGetDeviceQueue(this->context.device, indices.presentFamily.value(), 0, &this->context.present_queue);
}

void VulkanRenderer::delete_device_and_queues()
{
	vkDestroyDevice(this->context.device, nullptr);
}

void Genesis::VulkanRenderer::create_allocator()
{
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = this->context.device_properties.physical_device;
	allocatorInfo.device = this->context.device;

	vmaCreateAllocator(&allocatorInfo, &this->context.allocator);
}

void Genesis::VulkanRenderer::delete_allocator()
{
	vmaDestroyAllocator(this->context.allocator);
}

//Swapchain UTILS
VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkFormat findSupportedFormat(VkPhysicalDevice device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) 
{
	for (VkFormat format : candidates) 
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(device, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) 
		{
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) 
		{
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}

VkFormat findDepthFormat(VkPhysicalDevice device)
{
	return findSupportedFormat(device, { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, VkExtent2D current_extent)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = current_extent;
		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

void createImage(VmaAllocator& allocator, VkExtent2D size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VmaMemoryUsage memory_type, VkImage& image, VmaAllocation& image_memory)
{
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = size.width;
	imageInfo.extent.height = size.height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo depthImageAllocCreateInfo = {};
	depthImageAllocCreateInfo.usage = memory_type;
	vmaCreateImage(allocator, &imageInfo, &depthImageAllocCreateInfo, &image, &image_memory, nullptr);
}

VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) 
{
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create texture image view!");
	}

	return imageView;
}

void VulkanRenderer::create_swapchain()
{
	VulkanSwapChainSupportDetails swapChainSupport = VulkanSwapChainSupportDetails::querySwapChainSupport(this->context.device_properties.physical_device, this->context.surface);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);

	vector2U window_size = window->getWindowSize();
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, { window_size.x, window_size.y });

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = this->context.surface;

	create_info.minImageCount = imageCount;
	create_info.imageFormat = surfaceFormat.format;
	create_info.imageColorSpace = surfaceFormat.colorSpace;
	create_info.imageExtent = extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	VulkanQueueFamilyIndices indices = VulkanQueueFamilyIndices::findQueueFamilies(this->context.device_properties.physical_device, this->context.surface);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily)
	{
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0;
		create_info.pQueueFamilyIndices = nullptr;
	}

	create_info.preTransform = swapChainSupport.capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = presentMode;
	create_info.clipped = VK_TRUE;
	create_info.oldSwapchain = VK_NULL_HANDLE;

	VkResult result = vkCreateSwapchainKHR(this->context.device, &create_info, nullptr, &this->context.swapchain);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(this->context.device, this->context.swapchain, &imageCount, nullptr);
	this->context.swapchain_images.swapchain_images.resize(imageCount);
	vkGetSwapchainImagesKHR(this->context.device, this->context.swapchain, &imageCount, this->context.swapchain_images.swapchain_images.data());

	this->context.swapchain_image_count = imageCount;
	this->context.swapchain_properties.swapchain_image_format = surfaceFormat.format;
	this->context.swapchain_properties.swapchain_extent = extent;


	this->context.swapchain_images.swapchain_imageviews.resize(this->context.swapchain_image_count);
	for (size_t i = 0; i < this->context.swapchain_images.swapchain_images.size(); i++)
	{
		VkImageViewCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.image = this->context.swapchain_images.swapchain_images[i];
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		create_info.format = this->context.swapchain_properties.swapchain_image_format;
		create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = 1;

		if (vkCreateImageView(this->context.device, &create_info, nullptr, &this->context.swapchain_images.swapchain_imageviews[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image views!");
		}
	}

	//Depth Texture
	VkFormat depthFormat = findDepthFormat(this->context.device_properties.physical_device);
	createImage(this->context.allocator, this->context.swapchain_properties.swapchain_extent, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VMA_MEMORY_USAGE_GPU_ONLY, this->context.swapchain_images.depth_image, this->context.swapchain_images.depth_image_memory);
	this->context.swapchain_images.depth_imageview = createImageView(this->context.device, this->context.swapchain_images.depth_image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void VulkanRenderer::delete_swapchain()
{
	vkDestroySwapchainKHR(this->context.device, this->context.swapchain, nullptr);

	for (auto imageView : this->context.swapchain_images.swapchain_imageviews)
	{
		vkDestroyImageView(this->context.device, imageView, nullptr);
	}

	vkDestroyImageView(this->context.device, this->context.swapchain_images.depth_imageview, nullptr);
	vmaDestroyImage(this->context.allocator, this->context.swapchain_images.depth_image, this->context.swapchain_images.depth_image_memory);
}

void VulkanRenderer::create_screen_render_pass()
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = this->context.swapchain_properties.swapchain_image_format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = findDepthFormat(this->context.device_properties.physical_device);
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(this->context.device, &renderPassInfo, nullptr, &this->context.screen_render_pass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass!");
	}
}

void VulkanRenderer::delete_screen_render_pass()
{
	vkDestroyRenderPass(this->context.device, this->context.screen_render_pass, nullptr);
}

void VulkanRenderer::create_swapchain_framebuffers()
{
	this->context.swapchain_images.swapchain_framebuffers.resize(this->context.swapchain_images.swapchain_imageviews.size());

	for (size_t i = 0; i < this->context.swapchain_images.swapchain_imageviews.size(); i++)
	{
		Array<VkImageView> attachments(2);
		attachments[0] = this->context.swapchain_images.swapchain_imageviews[i];
		attachments[1] = this->context.swapchain_images.depth_imageview;

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = this->context.screen_render_pass;
		framebufferInfo.attachmentCount = (uint32_t) attachments.size();
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = this->context.swapchain_properties.swapchain_extent.width;
		framebufferInfo.height = this->context.swapchain_properties.swapchain_extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(this->context.device, &framebufferInfo, nullptr, &this->context.swapchain_images.swapchain_framebuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void VulkanRenderer::delete_swapchain_framebuffers()
{
	for (auto framebuffer : this->context.swapchain_images.swapchain_framebuffers)
	{
		vkDestroyFramebuffer(this->context.device, framebuffer, nullptr);
	}
}

void Genesis::VulkanRenderer::create_command_buffers()
{
	VulkanQueueFamilyIndices indices = VulkanQueueFamilyIndices::findQueueFamilies(this->context.device_properties.physical_device, nullptr);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = indices.graphicsFamily.value();
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(this->context.device, &poolInfo, nullptr, &this->context.command_pool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool!");
	}

	this->context.command_buffers.resize(this->context.swapchain_image_count);

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = this->context.command_pool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)this->context.command_buffers.size();

	if (vkAllocateCommandBuffers(this->context.device, &allocInfo, this->context.command_buffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

void Genesis::VulkanRenderer::delete_command_buffers()
{
	vkDestroyCommandPool(this->context.device, this->context.command_pool, nullptr);
}

void Genesis::VulkanRenderer::create_sync_objects()
{
	this->context.image_available_semaphores.resize(this->context.swapchain_image_count);
	this->context.render_finished_semaphores.resize(this->context.swapchain_image_count);
	this->context.in_flight_fences.resize(this->context.swapchain_image_count);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < this->context.swapchain_image_count; i++) 
	{
		if (vkCreateSemaphore(this->context.device, &semaphoreInfo, nullptr, &this->context.image_available_semaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(this->context.device, &semaphoreInfo, nullptr, &this->context.render_finished_semaphores[i]) != VK_SUCCESS ||
			vkCreateFence(this->context.device, &fenceInfo, nullptr, &this->context.in_flight_fences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

void Genesis::VulkanRenderer::delete_sync_objects()
{
	for (size_t i = 0; i < this->context.swapchain_image_count; i++)
	{
		vkDestroySemaphore(this->context.device, this->context.image_available_semaphores[i], nullptr);
		vkDestroySemaphore(this->context.device, this->context.render_finished_semaphores[i], nullptr);
		vkDestroyFence(this->context.device, this->context.in_flight_fences[i], nullptr);
	}
}

vector<const char*> VulkanRenderer::getExtensions()
{
	vector<const char*> extensions;

	extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

#ifdef VK_USE_PLATFORM_WIN32_KHR
	extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif


	if (this->use_debug_layers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	return extensions;
}

vector<const char*> VulkanRenderer::getDeviceExtensions()
{
	vector<const char*> extensions;

	extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	return extensions;
}

vector<const char*> VulkanRenderer::getLayers()
{
	vector<const char*> layers;

	if (this->use_debug_layers)
	{
		layers.push_back("VK_LAYER_LUNARG_standard_validation");
	}

	return layers;
}