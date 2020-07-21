#pragma once
namespace Genesis
{
	struct NameComponent
	{
		NameComponent(const char* name)
		{
			strcpy_s(this->data, name);
		}

		static const size_t SIZE = 32;
		char data[SIZE];
	};
}