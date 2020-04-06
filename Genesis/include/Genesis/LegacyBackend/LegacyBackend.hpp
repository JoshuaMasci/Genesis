#pragma once

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	typedef uint32_t Buffer;
	enum class BufferType
	{
		Vertex_Buffer,
		Index_Buffer
	};

	enum class TextureFormat
	{
		R,
		RG,
		RGB,
		RGBA
	};

	typedef uint32_t Texture2D;
	
	typedef void* ShaderProgram;

	class LegacyBackend
	{
	public:
		virtual ~LegacyBackend() {};

		virtual void startFrame() = 0;
		virtual void endFrame() = 0;

		virtual Buffer createBuffer(BufferType type, void* data, uint64_t data_size) = 0;
		virtual void destoryBuffer(Buffer buffer) = 0;

		virtual Texture2D createTexture(TextureFormat format, vector2F size, void* data, uint64_t data_size) = 0;
		virtual void destoryTexture(Texture2D texture) = 0;

		virtual ShaderProgram createShaderProgram(const char* vert_data, uint32_t vert_size, const char* frag_data, uint32_t frag_size) = 0;
		virtual void destoryShaderProgram(ShaderProgram program) = 0;

		//Commands
		virtual void bindShaderProgram(ShaderProgram program) = 0;
		virtual void setUniform1i(const string& name, const int32_t& value) = 0;
		virtual void setUniform1u(const string& name, const uint32_t& value) = 0;
		virtual void setUniform1f(const string& name, const float& value) = 0;
		virtual void setUniform2f(const string& name, const vector2F& value) = 0;
		virtual void setUniform3f(const string& name, const vector3F& value) = 0;
		virtual void setUniform4f(const string& name, const vector4F& value) = 0;
		virtual void setUniformMat3f(const string& name, const matrix3F& value) = 0;
		virtual void setUniformMat4f(const string& name, const matrix4F& value) = 0;

		virtual void draw(Buffer vertex_buffer, Buffer index_buffer, uint32_t triangle_count) = 0;
	};
}