#pragma once

namespace Genesis
{
	struct NameComponent
	{
		NameComponent()
		{
			//No name
			data[0] = '\0';
		}

		NameComponent(const char* name)
		{
			this->set(name);
		}

		void set(const char* name)
		{
			strcpy_s(this->data, name);
		}

		static const size_t SIZE = 32;
		char data[SIZE];
	};
}