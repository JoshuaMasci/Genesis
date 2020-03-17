#pragma once

#include "Genesis/Core/Types.hpp"

namespace Genesis
{
#define HASHING_PRIME 31

	typedef size_t StringHash;

	//Implementation From: https://xueyouchao.github.io/2016/11/16/CompileTimeString/
	template <size_t N>
	constexpr inline StringHash Horner_Hash_Compile(size_t prime, const char(&str)[N], size_t Len = N - 1)
	{
		return (Len <= 1) ? str[0] : (prime * Horner_Hash_Compile(prime, str, Len - 1) + str[Len - 1]);
	}

	StringHash Horner_Hash_Runtime(size_t prime, char const* str)
	{
		if (str == NULL) return 0;
		StringHash hash = *str;
		for (; *(str + 1) != 0; str++)
		{
			hash = prime * hash + *(str + 1);
		}
		return hash;
	}
}

#define COMPILE_STRING_HASH(x) (Genesis::Horner_Hash_Compile(HASHING_PRIME, x))
#define RUNTIME_STRING_HASH(x) (Genesis::Horner_Hash_Runtime(HASHING_PRIME, x))