#pragma once

#include "Genesis/Debug/Log.hpp"

#define GENESIS_ASSERT_ENABLED
#ifdef GENESIS_ASSERT_ENABLED

#define GENESIS_ENGINE_ASSERT(x, e) if(!(x)) { GENESIS_ENGINE_ERROR(e); GENESIS_ENGINE_ERROR("Assert {}:{}", __FILE__, __LINE__); abort(); };
#define GENESIS_ASSERT(x, e) if(!(x)) { GENESIS_ERROR(e); GENESIS_ERROR("Assert {}:{}", __FILE__, __LINE__); abort(); };

#else

#define GENESIS_ENGINE_ASSERT(x, e) 
#define GENESIS_ASSERT(x, e) 

#endif