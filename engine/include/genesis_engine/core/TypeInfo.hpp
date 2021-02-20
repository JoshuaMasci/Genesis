#pragma once

#include "genesis_engine/core/Fnv1aHash.hpp"

#if defined _MSC_VER
#   define GENERATOR_PRETTY_FUNCTION __FUNCSIG__
#elif defined __clang__ || (defined __GNUC__)
#   define GENERATOR_PRETTY_FUNCTION __PRETTY_FUNCTION__
#endif

namespace genesis_engine
{
	template<typename T>
	struct TypeInfo
	{
		static constexpr size_t getHash()
		{
			constexpr size_t value = (size_t)StringHash64(GENERATOR_PRETTY_FUNCTION);
			return value;
		}

		static constexpr size_t getSize()
		{
			constexpr auto size = sizeof(T);
		}
	};
}