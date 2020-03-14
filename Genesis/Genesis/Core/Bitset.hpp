#pragma once

#include <stdint.h>
#include <iostream>
#include "Genesis/Debug/Assert.hpp"

namespace Genesis
{
	class Bitset32
	{
	public:
		Bitset32(uint32_t data = 0);

		void set(size_t bit, bool value = true);
		bool get(size_t bit);
		size_t size();
		bool any();

		Bitset32 operator&(Bitset32 const &bitset) const;
		Bitset32 operator|(Bitset32 const &bitset) const;
		bool operator==(Bitset32 const &bitset) const;
		bool operator!=(Bitset32 const &bitset) const;
		bool operator>(Bitset32 const &bitset) const;
		bool operator<(Bitset32 const &bitset) const;

	private:
		uint32_t data;
	};

	class Bitset64
	{
	public:
		Bitset64(uint64_t data = 0);

		void set(size_t bit, bool value = true);
		bool get(size_t bit);
		size_t size();
		bool any();

		Bitset64 operator&(Bitset64 const &bitset) const;
		Bitset64 operator|(Bitset64 const &bitset) const;
		bool operator==(Bitset64 const &bitset) const;
		bool operator!=(Bitset64 const &bitset) const;
		bool operator>(Bitset64 const &bitset) const;
		bool operator<(Bitset64 const &bitset) const;

	private:
		uint64_t data;
	};

	class Bitset256
	{
	public:
		Bitset256(uint64_t data1 = 0, uint64_t data2 = 0, uint64_t data3 = 0, uint64_t data4 = 0);

		void set(size_t bit, bool value = true);
		bool get(size_t bit);
		size_t size();
		bool any();

		Bitset256 operator&(Bitset32 const &bitset) const;
		Bitset256 operator|(Bitset32 const &bitset) const;
		bool operator==(Bitset32 const &bitset) const;
		bool operator!=(Bitset32 const &bitset) const;
		bool operator>(Bitset32 const &bitset) const;
		bool operator<(Bitset32 const &bitset) const;

	protected:
		Bitset64 set1;
		Bitset64 set2;
		Bitset64 set3;
		Bitset64 set4;
	};
}