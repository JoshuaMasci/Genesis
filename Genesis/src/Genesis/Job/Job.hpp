#pragma once

#include <future>

namespace Genesis
{
	class Job
	{
	public:
		Job()
		{
			this->finish_future = this->finish_promise.get_future();
		};

		virtual void run() = 0;

		void finish()
		{
			this->finish_promise.set_value(true);//May allow to change value later
		};

		bool IsFinished()
		{
			return this->finish_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
		};

		bool waitTillFinished() 
		{ 
			return this->finish_future.get(); 
		};

		//Job* getNext(); TODO use for Job Fibers

	private:
		std::promise<bool> finish_promise;
		std::future<bool> finish_future;
	};
};