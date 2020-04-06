#pragma once

#include "Genesis/LegacyBackend/LegacyBackend.hpp"
#include "OpenglShaderProgram.hpp"
#include "SDL2_Window.hpp"

namespace Genesis
{
	namespace Opengl
	{
		class OpenglBackend : public LegacyBackend
		{
		public:
			OpenglBackend(SDL2_Window* window);
			virtual ~OpenglBackend();

			virtual void startFrame() override;
			virtual void endFrame() override;

			virtual Buffer createBuffer(BufferType type, void* data, uint64_t data_size) override;
			virtual void destoryBuffer(Buffer buffer) override;

			virtual Texture2D createTexture(TextureFormat format, vector2F size, void* data, uint64_t data_size) override;
			virtual void destoryTexture(Texture2D texture) override;

			virtual ShaderProgram createShaderProgram(const char* vert_data, uint32_t vert_size, const char* frag_data, uint32_t frag_size) override;
			virtual void destoryShaderProgram(ShaderProgram program) override;

			virtual void bindShaderProgram(ShaderProgram program) override;
			virtual void setUniform1i(const string& name, const int32_t& value) override;
			virtual void setUniform1u(const string& name, const uint32_t& value) override;
			virtual void setUniform1f(const string& name, const float& value) override;
			virtual void setUniform2f(const string& name, const vector2F& value) override;
			virtual void setUniform3f(const string& name, const vector3F& value) override;
			virtual void setUniform4f(const string& name, const vector4F& value) override;
			virtual void setUniformMat3f(const string& name, const matrix3F& value) override;
			virtual void setUniformMat4f(const string& name, const matrix4F& value) override;

			virtual void draw(Buffer vertex_buffer, Buffer index_buffer, uint32_t triangle_count) override;

		protected:
			SDL2_Window* window;
			void* opengl_context;

			OpenglShaderProgram* current_program = nullptr;
		};
	}
}