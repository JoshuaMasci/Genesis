#pragma once

#include "Genesis/Rendering/RenderingBackend.hpp"
#include "Genesis/Platform/Window.hpp"

namespace Genesis
{
	//Prototype
	class VulkanInstance;

	class VulkanBackend : public RenderingBackend
	{
	public:
		VulkanBackend(Window* window);
		~VulkanBackend();

		virtual uint32_t createImage(vector2U size) override;
		virtual Buffer* createBuffer(uint32_t size_bytes, BufferType type, MemoryUsage memory_usage) override;

	private:
		VulkanInstance* vulkan = nullptr;
	};
}