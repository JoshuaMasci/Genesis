#pragma once

#include "genesis_core/types/containers.hpp"

namespace genesis
{
	typedef uint32_t ResourceId;
	const ResourceId NullResource = UINT32_MAX;

	class RenderBackend {
	public:

		//Resource Creation
		void create_sync_resource(const string& name) {};
		void create_storage_buffer() {};


	};
}