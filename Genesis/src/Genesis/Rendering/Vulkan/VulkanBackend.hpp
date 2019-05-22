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
		VulkanBackend(Window* window, uint32_t number_of_threads);
		~VulkanBackend();

		virtual void beginFrame() override;
		virtual void endFrame() override;

		virtual Buffer* createBuffer(uint64_t size_bytes, BufferType type, MemoryUsage memory_usage) override;

	private:
		VulkanInstance* vulkan = nullptr;
	};
}