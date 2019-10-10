#pragma once

#include <stdint.h>

namespace Genesis
{
	class Job
	{
	public:
		Job()
		{
			
		};

		virtual void run(uint32_t thread_id) = 0;

		void finish()
		{
			this->finished = true;
		};

		bool IsFinished()
		{
			return this->finished;
		};

		void waitTillFinished() 
		{ 
			//Spin lock
			while (!this->IsFinished())
			{
				//TODO maybe sleep
			}
		};

		//Job* getNext(); TODO use for Job Fibers

	private:
		//Finished Flag
		//Using a bool because it will only ever be written from one thread and read from another
		//Because there is no case that 2 threads could ever write to it at the same time, so there is no reason to put a lock on it.
		bool finished = false;
	};
};