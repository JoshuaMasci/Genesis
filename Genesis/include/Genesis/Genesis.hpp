#pragma once

#include <Genesis/Core/Application.hpp>

//DEBUG
#include "Genesis/Debug/Log.hpp"
#include "Genesis/Debug/Profiler.hpp"
#include "Genesis/Debug/Assert.hpp"

#define GENESIS_ENGINE_DEBUG_START() {	GENESIS_PROFILE_START(); Genesis::Logging::inti_engine_logging(""); };