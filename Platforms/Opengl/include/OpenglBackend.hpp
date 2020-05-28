#pragma once

#include "Genesis/LegacyBackend/LegacyBackend.hpp"
#include "OpenglShaderProgram.hpp"
#include "SDL2_Window.hpp"

namespace Genesis
{
	namespace Opengl
	{
		struct OpenglVertexBuffer
		{
			GLuint vertex_array_object;
			GLuint vertex_buffer;
		};

		struct OpenglIndexBuffer
		{
			GLuint index_buffer;
			IndexType type;
		};

		struct OpenglTexture2D
		{
			GLuint texture_handle;
		};

		struct OpenglFramebuffer
		{
			vector2U size;
			bool is_multisampled;

			GLuint frame_buffer;
			vector<OpenglTexture2D> attachements;

			bool has_depth;
			OpenglTexture2D depth_attachement;
		};

		class OpenglBackend : public LegacyBackend
		{
		public:
			OpenglBackend(SDL2_Window* window);
			virtual ~OpenglBackend();

			virtual vector2U getScreenSize() override;

			virtual void startFrame() override;
			virtual void endFrame() override;

			virtual VertexBuffer createVertexBuffer(void* data, uint64_t data_size, const VertexInputDescriptionCreateInfo& vertex_description) override;
			virtual void destoryVertexBuffer(VertexBuffer buffer) override;

			virtual IndexBuffer createIndexBuffer(void* data, uint64_t data_size, IndexType type) override;
			virtual void destoryIndexBuffer(IndexBuffer buffer) override;

			virtual Texture2D createTexture(const TextureCreateInfo& create_info, void* data) override;
			virtual void destoryTexture(Texture2D texture) override;

			virtual ShaderProgram createShaderProgram(const char* vert_data, uint32_t vert_size, const char* frag_data, uint32_t frag_size) override;
			virtual void destoryShaderProgram(ShaderProgram program) override;

			virtual Framebuffer createFramebuffer(const FramebufferCreateInfo& create_info) override;
			virtual void destoryFramebuffer(Framebuffer framebuffer) override;
			virtual Texture2D getFramebufferColorAttachment(Framebuffer framebuffer, uint32_t index) override;
			virtual Texture2D getFramebufferDepthAttachment(Framebuffer framebuffer) override;

			virtual void bindFramebuffer(Framebuffer framebuffer) override;
			virtual void clearFramebuffer(bool color, bool depth, vector4F* clear_color = nullptr, float* clear_depth = nullptr) override;

			virtual void setPipelineState(const PipelineSettings& pipeline_state) override;

			virtual void bindShaderProgram(ShaderProgram program) override;
			virtual void setUniform1i(const string& name, const int32_t& value) override;

			virtual void setUniform1u(const string& name, const uint32_t& value) override;
			virtual void setUniform2u(const string& name, const vector2U& value) override;
			virtual void setUniform3u(const string& name, const vector3U& value) override;
			virtual void setUniform4u(const string& name, const vector4U& value) override;

			virtual void setUniform1f(const string& name, const float& value) override;
			virtual void setUniform2f(const string& name, const vector2F& value) override;
			virtual void setUniform3f(const string& name, const vector3F& value) override;
			virtual void setUniform4f(const string& name, const vector4F& value) override;
			virtual void setUniformMat3f(const string& name, const matrix3F& value) override;
			virtual void setUniformMat4f(const string& name, const matrix4F& value) override;
			virtual void setUniformTexture(const string& name, const uint32_t texture_slot, Texture2D value) override;

			virtual void setScissor(vector2I offset, vector2U extent) override;
			virtual void clearScissor() override;

			virtual void bindVertexBuffer(VertexBuffer buffer) override;
			virtual void bindIndexBuffer(IndexBuffer buffer) override;
			virtual void drawIndex(uint32_t index_count, uint32_t index_offset = 0) override;

			virtual void draw(VertexBuffer vertex_buffer, IndexBuffer index_buffer, uint32_t triangle_count) override;

			virtual FrameStats getLastFrameStats() override;

		protected:
			SDL2_Window* window;
			void* opengl_context;
			vector2U viewport_size;

			OpenglShaderProgram* current_program = nullptr;
			OpenglVertexBuffer* vertex_buffer = nullptr;
			OpenglIndexBuffer* index_buffer = nullptr;

			//Stats
			FrameStats last_frame_stats;
			FrameStats current_frame_stats;
		};
	}
}