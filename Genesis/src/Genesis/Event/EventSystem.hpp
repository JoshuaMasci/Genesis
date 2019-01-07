#pragma once

#include "Genesis/Core/DLL.hpp"
#include "Genesis/Core/Types.hpp"

#include <memory>
#include <functional>

#define BindFunc(CLASS, EVENT, FUNC) std::bind((void(CLASS::*)(EVENT*))&CLASS::FUNC, &test, std::placeholders::_1)

#define EVENT_CALLBACK_MAP map<size_t, void*>

namespace Genesis
{
	class GENESIS_DLL EventSystem
	{
	public:
		EventSystem();
		~EventSystem();

		template<class E>
		void subscribe(std::function<void(E*)> func)
		{
			size_t event_hashcode = typeid(E).hash_code();

			if (this->event_callbacks->find(event_hashcode) == this->event_callbacks->end())
			{
				this->event_callbacks->emplace(std::pair<size_t, void*>(event_hashcode, (void*) new vector<std::function<void(E*)>>()));
			}

			void* void_ptr = this->event_callbacks->at(event_hashcode);
			vector<std::function<void(E*)>>* vector_ptr = (vector<std::function<void(E*)>>*) void_ptr;
			vector_ptr->push_back(func);
		}

		template<class E>
		void emit(E &e)
		{
			size_t event_hashcode = typeid(E).hash_code();
			if (this->event_callbacks->find(event_hashcode) != this->event_callbacks->end())
			{
				void* void_ptr = this->event_callbacks->at(event_hashcode);
				vector<std::function<void(E*)>>* vector_ptr = (vector<std::function<void(E*)>>*) void_ptr;

				for (std::function<void(E*)> func: *vector_ptr)
				{
					func(&e);
				}
			}
		}

	protected:
		EVENT_CALLBACK_MAP* event_callbacks;
	};
};