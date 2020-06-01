#pragma once

namespace Genesis
{
	struct NameComponent
	{
		static const size_t data_size = 128;

		NameComponent(const char* name)
		{
			strcpy_s(this->data, name);
		};

		char data[NameComponent::data_size];
	};
}