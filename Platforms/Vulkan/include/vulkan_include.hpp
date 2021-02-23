#pragma once

#define NOMINMAX

//TODO other platforms
#ifdef WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif // WIN32

#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"

#define VK_ASSERT(x) if(x != VK_SUCCESS) { printf("VulkanRendering Error: %s:%d\n", __FILE__, __LINE__); abort(); };

#include "genesis_core/types/containers.hpp"
#include "genesis_core/types/vector_math.hpp"