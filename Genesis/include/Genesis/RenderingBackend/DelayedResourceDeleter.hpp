#pragma once

#include "Genesis/Debug/Log.hpp"

#include <stdint.h>
#include <queue>

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
				GENESIS_ENGINE_WARN("Delay must be at least 1");
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
			Resource<T> res = { object, this->delay_cycles };
			if (this->queue_select)
			{
				this->queue_a.push(res);
			}
			else
			{
				this->queue_b.push(res);
			}
		};

		void cycle()
		{
			std::queue<Resource<T>>* read_queue;
			std::queue<Resource<T>>* write_queue;

			if (this->queue_select)
			{
				read_queue = &this->queue_a;
				write_queue = &this->queue_b;
			}
			else
			{
				read_queue = &this->queue_b;
				write_queue = &this->queue_a;
			}

			while (!read_queue->empty())
			{
				Resource<T> res = read_queue->front();
				read_queue->pop();
				
				res.cycles_remaining--;

				if (res.cycles_remaining == 0)
				{
					//Delete object
					delete res.object;
				}
				else
				{
					write_queue->push(res);
				}
			}

			//Flip to next queue
			this->queue_select = !this->queue_select;
		};

		void flushAll()
		{
			//Empties both queues
			while (!this->queue_a.empty())
			{
				Resource<T> res = this->queue_a.front();
				this->queue_a.pop();
				delete res.object;
			}

			while (!this->queue_b.empty())
			{
				Resource<T> res = this->queue_b.front();
				this->queue_b.pop();
				delete res.object;
			}
		};

	private:
		uint8_t delay_cycles;

		bool queue_select = true; //true means a, false means b
		std::queue<Resource<T>> queue_a;
		std::queue<Resource<T>> queue_b;
	};
}