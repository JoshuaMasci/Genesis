#pragma once

#define has_value(list, value) (list.find(value) != list.end())

#include <stdint.h>
#include <bitset>

namespace Genesis
{
	namespace EntitySystem
	{
		template <typename T>
		struct TypeInfo
		{
			constexpr static size_t getHash()
			{
				return typeid(T).hash_code();
			}

			constexpr static size_t getSize()
			{
				return sizeof(T);
			};
		};
	};

	typedef uint64_t EntityHandle;
	typedef uint32_t ComponentId;
	typedef std::bitset<64> EntitySignature;
}