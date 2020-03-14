#pragma once

#include <stdint.h>
#include "Genesis/Core/Bitset.hpp"

namespace Genesis
{
	namespace Ecs
	{
		template <typename T>
		struct TypeInfo
		{
			static size_t getHash()
			{
				return typeid(T).hash_code();
			}

			static size_t getSize()
			{
				return sizeof(T);
			};
		};
	};

	typedef uint64_t EntityHandle;
	typedef uint32_t ComponentId;
	typedef Bitset64 EntitySignature;
}