#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Debug/Log.hpp"

#include <stdint.h>

namespace Genesis
{
	/*
		DelayedResourceDeleter keeps track of graphics objects that need to be deleted
		It will wait x number of cycles before actually deleting the object
		This is so the object can be completely flushed from the graphics pipeline before being deleted
		NOT THREAD SAFE
	*/
	template<typename T>
	class DelayedResourceDeleter
	{
		template<class T>
		struct Resource
		{
			T* object = nullptr;
			uint8_t cycles_remaining = 0;
		};

	public:
		DelayedResourceDeleter(uint8_t delay_cycles)
		{
			if (delay_cycles == 0)
			{
				GENESIS_ENGINE_WARNING("Delay must be at least 1");
				delay_cycles = 1;
			}

			this->delay_cycles = delay_cycles;
		};

		~DelayedResourceDeleter()
		{
			this->flushAll();
		};

		void addToQueue(T* object)
		{
			this->resources.push_back({ object, this->delay_cycles });
		};

		void cycle()
		{
			for (size_t i = 0; i < this->resources.size(); i++)
			{
				if (this->resources[i].cycles_remaining == 0)
				{
					delete this->resources[i].object;

					size_t last_index = (this->resources.size() - 1);

					//If is not in last index
					if (i != last_index)
					{
						//Swap the last element with deleted element
						this->resources[i] = this->resources[last_index];
					}

					//Remove last element
					this->resources.pop_back();
				}
				else
				{
					this->resources[i].cycles_remaining--;
				}
			}
		};

		void flushAll()
		{
			for (size_t i = 0; i < this->resources.size(); i++)
			{
				delete this->resources[i].object;
			}
		};

	private:
		uint8_t delay_cycles;

		vector<Resource<T>> resources;
	};
}