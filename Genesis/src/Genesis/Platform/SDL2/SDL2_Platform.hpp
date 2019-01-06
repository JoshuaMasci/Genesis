#pragma once

#include "Genesis/Platform/Platform.hpp"

namespace Genesis
{
	class SDL2_Platform : public Genesis::Platform
	{
	public:
		SDL2_Platform();
		virtual ~SDL2_Platform();

		virtual void onUpdate(double delta_time) override;
		
	private:
		void* input_devices;
	};
}