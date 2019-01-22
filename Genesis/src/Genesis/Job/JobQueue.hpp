#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

namespace Genesis
{
	//A queue that can be used by muiltiple threads
	template<typename T>
	class JobQueue
	{
	public:
		void push(T element)
		{
			std::lock_guard<std::mutex>lock(this->m_mutex);
			this->m_queue.push(element);
			this->m_cond.notify_one();
		}

		T pop()
		{
			std::unique_lock<std::mutex>lock(this->m_mutex);
			this->m_cond.wait(lock, [&] { return !m_queue.empty(); });
			T element = this->m_queue.front();
			this->m_queue.pop();
			return element;
		}

	private:
		std::queue<T> m_queue;
		std::mutex m_mutex;
		std::condition_variable m_cond;
	};
};