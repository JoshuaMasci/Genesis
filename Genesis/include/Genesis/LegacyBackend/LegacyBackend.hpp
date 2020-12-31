#pragma once

#include "Genesis/RenderingBackend/VertexInputDescription.hpp"
#include "Genesis/RenderingBackend/RenderingTypes.hpp"
#include "Genesis/RenderingBackend/PipelineSettings.hpp"

namespace Genesis
{
	enum class DepthFormat
	{
		depth_16,
		depth_24,
		depth_32,
		depth_32f,
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
		ImageFormat format;
		TextureWrapMode wrap_mode;
		TextureFilterMode filter_mode;
	};

	enum class MultisampleCount
	{
		Sample_1 = 1,
		Sample_2 = 2,
		Sample_4 = 4,
		Sample_8 = 8,
		Sample_16 = 16,
		Sample_32 = 32,
		Sample_64 = 64
	};

	struct FramebufferAttachmentInfo
	{
		ImageFormat format;
		MultisampleCount samples;
	};

	struct FramebufferDepthInfo
	{
		DepthFormat format;
		MultisampleCount samples;
	};

	struct FramebufferCreateInfo
	{
		vector2U size;
		FramebufferAttachmentInfo* attachments;
		uint32_t attachment_count;
		FramebufferDepthInfo* depth_attachment;
	};
	typedef void* Framebuffer;
	
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
		virtual ShaderProgram createComputeShader(const char* data, uint32_t size) = 0;
		virtual void destoryShaderProgram(ShaderProgram program) = 0;

		virtual Framebuffer createFramebuffer(const FramebufferCreateInfo& create_info) = 0;
		virtual void destoryFramebuffer(Framebuffer framebuffer) = 0;
		virtual Texture2D getFramebufferColorAttachment(Framebuffer framebuffer, uint32_t index) = 0;
		virtual Texture2D getFramebufferDepthAttachment(Framebuffer framebuffer) = 0;

		//Commands
		virtual void bindFramebuffer(Framebuffer framebuffer) = 0;
		virtual void clearFramebuffer(bool color, bool depth, vector4F* clear_color = nullptr, float* clear_depth = nullptr) = 0;

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
		
		virtual void setUniformTexture(const string& name, const uint32_t texture_slot, Texture2D value) = 0;
		virtual void setUniformTextureImage(const string& name, const uint32_t texture_slot, Texture2D value) = 0;

		virtual void setScissor(vector2I offset, vector2U extent) = 0;
		virtual void clearScissor() = 0;

		virtual void bindVertexBuffer(VertexBuffer buffer) = 0;
		virtual void bindIndexBuffer(IndexBuffer buffer) = 0;
		virtual void drawIndex(uint32_t index_count, uint32_t index_offset = 0) = 0;

		virtual void draw(VertexBuffer vertex_buffer, IndexBuffer index_buffer, uint32_t triangle_count) = 0;

		virtual void dispatchCompute(uint32_t groups_x = 1, uint32_t groups_y = 1, uint32_t groups_z = 1) = 0;

		virtual void blitFramebuffer(Framebuffer source, Framebuffer target) = 0;

		//Stats
		virtual FrameStats getLastFrameStats() = 0;
	};
}