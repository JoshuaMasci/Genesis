#pragma once

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/Core/Types.hpp"

#include "Genesis/RenderingBackend/VertexInputDescription.hpp"
#include "Genesis/RenderingBackend/RenderingTypes.hpp"
#include "Genesis/RenderingBackend/PipelineSettings.hpp"

namespace Genesis
{
	typedef void* VertexBuffer;
	typedef void* IndexBuffer;

	enum class TextureFormat
	{
		R = 1,
		RG = 2,
		RGB = 3,
		RGBA = 4
	};

	enum class TextureWrapMode
	{
		Repeat,
		Mirrored_Repeat,
		Clamp_Edge,
		Clamp_Border
	};

	enum class TextureFilterMode
	{
		Nearest,
		Linear
	};

	struct TextureCreateInfo
	{
		vector2U size;
		TextureFormat format;
		TextureWrapMode wrap_mode;
		TextureFilterMode filter_mode;
	};

	typedef uint32_t Texture2D;
	
	typedef void* ShaderProgram;

	struct FrameStats
	{
		uint64_t draw_calls;
		uint64_t triangles_count;
	};

	class LegacyBackend
	{
	public:
		virtual ~LegacyBackend() {};

		virtual vector2U getScreenSize() = 0;

		virtual void startFrame() = 0;
		virtual void endFrame() = 0;

		virtual VertexBuffer createVertexBuffer(void* data, uint64_t data_size, const VertexInputDescriptionCreateInfo& vertex_description) = 0;
		virtual void destoryVertexBuffer(VertexBuffer buffer) = 0;

		virtual IndexBuffer createIndexBuffer(void* data, uint64_t data_size, IndexType type) = 0;
		virtual void destoryIndexBuffer(IndexBuffer buffer) = 0;

		virtual Texture2D createTexture(const TextureCreateInfo& create_info, void* data) = 0;
		virtual void destoryTexture(Texture2D texture) = 0;

		virtual ShaderProgram createShaderProgram(const char* vert_data, uint32_t vert_size, const char* frag_data, uint32_t frag_size) = 0;
		virtual void destoryShaderProgram(ShaderProgram program) = 0;

		//Commands
		virtual void setPipelineState(const PipelineSettings& pipeline_state) = 0;

		virtual void bindShaderProgram(ShaderProgram program) = 0;
		virtual void setUniform1i(const string& name, const int32_t& value) = 0;

		virtual void setUniform1u(const string& name, const uint32_t& value) = 0;
		virtual void setUniform2u(const string& name, const vector2U& value) = 0;
		virtual void setUniform3u(const string& name, const vector3U& value) = 0;
		virtual void setUniform4u(const string& name, const vector4U& value) = 0;
		
		virtual void setUniform1f(const string& name, const float& value) = 0;
		virtual void setUniform2f(const string& name, const vector2F& value) = 0;
		virtual void setUniform3f(const string& name, const vector3F& value) = 0;
		virtual void setUniform4f(const string& name, const vector4F& value) = 0;
		
		virtual void setUniformMat3f(const string& name, const matrix3F& value) = 0;
		virtual void setUniformMat4f(const string& name, const matrix4F& value) = 0;
		
		virtual void setUniformTexture(const string& name, const uint32_t texture_slot, const Texture2D& value) = 0;

		virtual void setScissor(vector2I offset, vector2U extent) = 0;
		virtual void clearScissor() = 0;

		virtual void bindVertexBuffer(VertexBuffer buffer) = 0;
		virtual void bindIndexBuffer(IndexBuffer buffer) = 0;
		virtual void drawIndex(uint32_t index_count, uint32_t index_offset = 0) = 0;

		virtual void draw(VertexBuffer vertex_buffer, IndexBuffer index_buffer, uint32_t triangle_count) = 0;

		//Stats
		virtual FrameStats getLastFrameStats() = 0;
	};
}