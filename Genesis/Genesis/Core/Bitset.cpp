#include "Bitset.hpp"

using namespace Genesis;

//Bitset32 START
Bitset32::Bitset32(uint32_t data)
{
	this->data = data;
}

void Bitset32::set(size_t bit, bool value)
{
	uint32_t mask = 1 << bit;
	if (value)
	{
		this->data |= mask;
	}
	else
	{
		this->data &= ~mask;
	}
}

bool Bitset32::get(size_t bit)
{
	uint32_t mask = 1 << bit;
	uint32_t value = this->data & mask;
	return value > 0;
}

size_t Bitset32::size()
{
	return 32;
}

bool Bitset32::any()
{
	return this->data != 0;
}

Bitset32 Bitset32::operator&(Bitset32 const& bitset) const
{
	 return this->data & bitset.data;
}

Bitset32 Bitset32::operator|(Bitset32 const& bitset) const
{
	return this->data | bitset.data;
}

bool Bitset32::operator==(Bitset32 const& bitset) const
{
	return this->data == bitset.data;
}

bool Bitset32::operator!=(Bitset32 const& bitset) const
{
	return !(*this == bitset);
}

bool Bitset32::operator>(Bitset32 const& bitset) const
{
	return this->data > bitset.data;
}

bool Bitset32::operator<(Bitset32 const& bitset) const
{
	return this->data < bitset.data;
}

//Bitset64 START
Bitset64::Bitset64(uint64_t data)
{
	this->data = data;
}

void Bitset64::set(size_t bit, bool value)
{
	uint64_t mask = uint64_t(1) << bit;
	if (value)
	{
		this->data |= mask;
	}
	else
	{
		this->data &= ~mask;
	}
}

bool Bitset64::get(size_t bit)
{
	uint64_t mask = uint64_t(1) << bit;
	uint64_t value = this->data & mask;
	return value > 0;
}

size_t Bitset64::size()
{
	return 64;
}

bool Bitset64::any()
{
	return this->data != 0;
}

Bitset64 Bitset64::operator&(Bitset64 const& bitset) const
{
	return this->data & bitset.data;
}

Bitset64 Bitset64::operator|(Bitset64 const& bitset) const
{
	return this->data | bitset.data;
}

bool Bitset64::operator==(Bitset64 const& bitset) const
{
	return this->data == bitset.data;
}

bool Bitset64::operator!=(Bitset64 const& bitset) const
{
	return !(*this == bitset);
}

bool Bitset64::operator>(Bitset64 const& bitset) const
{
	return this->data > bitset.data;
}

bool Bitset64::operator<(Bitset64 const& bitset) const
{
	return this->data < bitset.data;
}

//Bitset256 START
Genesis::Bitset256::Bitset256(uint64_t data1, uint64_t data2, uint64_t data3, uint64_t data4)
:set1(data1), set2(data2), set3(data3), set4(data4){}

void Genesis::Bitset256::set(size_t bit, bool value)
{
	size_t index = bit / set1.size();
	switch (index)
	{
	case 0:
		set1.set(bit % set1.size());
		break;
	case 1:
		set2.set(bit % set1.size());
		break;
	case 2:
		set3.set(bit % set1.size());
		break;
	case 3:
		set4.set(bit % set1.size());
		break;
	}
}

bool Genesis::Bitset256::get(size_t bit)
{
	size_t index = bit / set1.size();
	switch (index)
	{
	case 0:
		return set1.get(bit % set1.size());
		break;
	case 1:
		return set2.get(bit % set1.size());
		break;
	case 2:
		return set3.get(bit % set1.size());
		break;
	case 3:
		return set4.get(bit % set1.size());
		break;
	}

	return false;
}

size_t Genesis::Bitset256::size()
{
	return 256;
}

bool Genesis::Bitset256::any()
{
	return set1.any() | set2.any() | set3.any() | set4.any();
}

Bitset256 Genesis::Bitset256::operator&(Bitset32 const& bitset) const
{
	return Bitset256();
}

Bitset256 Genesis::Bitset256::operator|(Bitset32 const& bitset) const
{
	return Bitset256();
}

bool Genesis::Bitset256::operator==(Bitset32 const& bitset) const
{
	return false;
}

bool Genesis::Bitset256::operator!=(Bitset32 const& bitset) const
{
	return false;
}

bool Genesis::Bitset256::operator>(Bitset32 const& bitset) const
{
	return false;
}

bool Genesis::Bitset256::operator<(Bitset32 const& bitset) const
{
	return false;
}