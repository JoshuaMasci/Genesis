#pragma once

namespace Genesis
{
	class Job
	{
	public:
		virtual void run() = 0;
		virtual void finish() = 0;

		//Job* getNext();

	private:
	};
};