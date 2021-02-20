#pragma once

#include "genesis_engine/LegacyBackend/LegacyBackend.hpp"
#include "OpenglShaderProgram.hpp"
#include "SDL2_Window.hpp"

namespace genesis_engine
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

		struct GLImageFormat
		{
			GLenum internal_format = 0;
			GLenum format = 0;
			GLenum type = 0;
		};

		struct OpenglTexture2D
		{
			GLuint texture_handle;
			GLImageFormat format;
		};

		struct OpenglFramebuffer
		{
			vec2u size;
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

			virtual vec2u getScreenSize() override;

			virtual void startFrame() override;
			virtual void endFrame() override;

			virtual VertexBuffer createVertexBuffer(void* data, uint64_t data_size, const VertexInputDescriptionCreateInfo& vertex_description) override;
			virtual void destoryVertexBuffer(VertexBuffer buffer) override;

			virtual IndexBuffer createIndexBuffer(void* data, uint64_t data_size, IndexType type) override;
			virtual void destoryIndexBuffer(IndexBuffer buffer) override;

			virtual Texture2D createTexture(const TextureCreateInfo& create_info, void* data) override;
			virtual void destoryTexture(Texture2D texture) override;

			virtual ShaderProgram createShaderProgram(const char* vert_data, uint32_t vert_size, const char* frag_data, uint32_t frag_size) override;
			virtual ShaderProgram createComputeShader(const char* data, uint32_t size) override;
			virtual void destoryShaderProgram(ShaderProgram program) override;

			virtual Framebuffer createFramebuffer(const FramebufferCreateInfo& create_info) override;
			virtual void destoryFramebuffer(Framebuffer framebuffer) override;
			virtual Texture2D getFramebufferColorAttachment(Framebuffer framebuffer, uint32_t index) override;
			virtual Texture2D getFramebufferDepthAttachment(Framebuffer framebuffer) override;

			virtual void bindFramebuffer(Framebuffer framebuffer) override;
			virtual void clearFramebuffer(bool color, bool depth, vec4f* clear_color = nullptr, float* clear_depth = nullptr) override;

			virtual void setPipelineState(const PipelineSettings& pipeline_state) override;

			virtual void bindShaderProgram(ShaderProgram program) override;
			virtual void setUniform1i(const string& name, const int32_t& value) override;

			virtual void setUniform1u(const string& name, const uint32_t& value) override;
			virtual void setUniform2u(const string& name, const vec2u& value) override;
			virtual void setUniform3u(const string& name, const vec3u& value) override;
			virtual void setUniform4u(const string& name, const vec4u& value) override;

			virtual void setUniform1f(const string& name, const float& value) override;
			virtual void setUniform2f(const string& name, const vec2f& value) override;
			virtual void setUniform3f(const string& name, const vec3f& value) override;
			virtual void setUniform4f(const string& name, const vec4f& value) override;
			virtual void setUniformMat3f(const string& name, const mat3f& value) override;
			virtual void setUniformMat4f(const string& name, const mat4f& value) override;
			virtual void setUniformTexture(const string& name, const uint32_t texture_slot, Texture2D value) override;
			virtual void setUniformTextureImage(const string& name, const uint32_t texture_slot, Texture2D value) override;

			virtual void setScissor(vec2i offset, vec2u extent) override;
			virtual void clearScissor() override;

			virtual void bindVertexBuffer(VertexBuffer buffer) override;
			virtual void bindIndexBuffer(IndexBuffer buffer) override;
			virtual void drawIndex(uint32_t index_count, uint32_t index_offset = 0) override;

			virtual void draw(VertexBuffer vertex_buffer, IndexBuffer index_buffer, uint32_t triangle_count) override;

			virtual void dispatchCompute(uint32_t groups_x = 1, uint32_t groups_y = 1, uint32_t groups_z = 1) override;

			virtual FrameStats getLastFrameStats() override;

		protected:
			SDL2_Window* window;
			void* opengl_context;
			vec2u viewport_size;

			OpenglShaderProgram* current_program = nullptr;
			OpenglVertexBuffer* vertex_buffer = nullptr;
			OpenglIndexBuffer* index_buffer = nullptr;

			//Stats
			FrameStats last_frame_stats;
			FrameStats current_frame_stats;
		};
	}
}