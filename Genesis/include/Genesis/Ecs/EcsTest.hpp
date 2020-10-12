#pragma once

namespace Genesis
{
	namespace Experimental
	{
		struct generator
		{
			static std::size_t next()
			{
				static std::size_t value{};
				return value++;
			}
		};

		template <typename T>
		struct TypeInfo
		{
			static size_t getIndex()
			{
				static const std::size_t value = generator::next();
				return value;
			}

			static size_t getHash()
			{
				return typeid(T).hash_code();
			}

			static size_t getSize()
			{
				return sizeof(T);
			}
		};
	}
}