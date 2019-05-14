#pragma once

#define NOMINMAX

#ifdef GENESIS_PLATFORM_WIN
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"