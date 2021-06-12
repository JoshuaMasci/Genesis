#pragma once

#include <genesis_core/types/containers.hpp>
#include "vulkan_renderer/buffer.hpp"

namespace genesis
{
	class MeshBufferVector
	{
	protected:
		struct DeletedBuffer
		{
			size_t index;
			Buffer* buffer;
		};

		vector<Buffer*> buffers;
		vector<uint32_t> freed_indices;

		vector<DeletedBuffer> buffers_to_delete;

	public:
		size_t insert(Buffer* buffer)
		{
			size_t new_index = 0;

			if (this->buffers_to_delete.empty())
			{
				new_index = this->buffers.size();
				this->buffers.push_back(buffer);
			}
			else
			{
				new_index = this->freed_indices.back();
				this->freed_indices.pop_back();
				this->buffers[new_index] = buffer;
			}

			return new_index;
		};

		void erase(size_t index)
		{
			this->buffers_to_delete.push_back({ index, this->buffers[index] });
			this->buffers[index] = nullptr;
		};

		void flush()
		{
			for (auto deleted_buffer : buffers_to_delete)
			{
				delete deleted_buffer.buffer;
				this->freed_indices.push_back(deleted_buffer.index);
			}
			this->buffers_to_delete.clear();
			this->buffers_to_delete.shrink_to_fit();
		};
	};
}