#pragma once

#include <inttypes.h>

//Disable the warning for integral constat overflow
#pragma warning( disable : 4307)
namespace genesis_types
{
	typedef uint32_t fnv_hash32;
	typedef uint64_t fnv_hash64;

	constexpr size_t str_length(const char* str)
	{
		return *str ? 1 + str_length(str + 1) : 0;
	}

	constexpr fnv_hash32 fnv1a_32(char const* s, size_t count)
	{
		const fnv_hash32 fnv_prime = 0x01000193;
		const fnv_hash32 fnv_seed = 0x811c9dc5;
		return ((count ? fnv1a_32(s, count - 1) : fnv_seed) ^ s[count]) * fnv_prime;
	}

	constexpr fnv_hash64 fnv1a_64(char const* s, size_t count)
	{
		const fnv_hash64 fnv_prime = 0x00000100000001B3;
		const fnv_hash64 fnv_seed = 0xcbf29ce484222325;
		return ((count ? fnv1a_64(s, count - 1) : fnv_seed) ^ s[count]) * fnv_prime;
	}

	constexpr fnv_hash32 string_hash_32(const char* string)
	{
		return fnv1a_32(string, str_length(string));
	}

	constexpr fnv_hash64 string_hash_64(const char* string)
	{
		return fnv1a_64(string, str_length(string));
	}
}