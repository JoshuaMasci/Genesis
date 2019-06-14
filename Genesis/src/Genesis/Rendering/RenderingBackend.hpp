#pragma once

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/Rendering/RenderingTypes.hpp"

namespace Genesis
{
	enum class MemoryUsage
	{
		GPU_Only,
		CPU_Visable
	};

	class RenderingBackend
	{
	public:
		virtual ~RenderingBackend() {};

		virtual bool beginFrame() = 0;
		virtual void endFrame() = 0;

		virtual BufferIndex createBuffer(uint64_t size_bytes, BufferType type, MemoryUsage memory_usage) = 0;
		virtual void fillBuffer(BufferIndex buffer_index, void* data, uint64_t data_size) = 0;
		virtual void destroyBuffer(BufferIndex buffer_index) = 0;

		virtual TextureIndex createTexture(vector2U size) = 0;
		virtual void fillTexture(TextureIndex texture_index, void* data, uint64_t data_size) = 0;
		virtual void destroyTexture(TextureIndex texture_index) = 0;

		//TEMP
		virtual void drawMeshScreen(uint32_t thread, BufferIndex vertices_index, BufferIndex indices_index, TextureIndex texture_index, uint32_t indices_count, matrix4F mvp) = 0;

		virtual matrix4F getPerspectiveMatrix(float frame_of_view, float aspect_ratio, float z_near) = 0;
		//virtual matrix4F getorthographicMatrix();

		virtual vector2U getScreenSize() = 0;

		//Wait until all GPU proccessing is done
		virtual void waitTillDone() = 0;

	private:
		uint32_t current_frame = 0;
	};
}