#include "OpenglBackend.hpp"

#include <Genesis/Debug/Assert.hpp>
#include <Genesis/Debug/Log.hpp>

#include "Opengl_Include.hpp"

using namespace Genesis;
using namespace Genesis::Opengl;

OpenglBackend::OpenglBackend(SDL2_Window* window)
{
	this->window = window;

	this->opengl_context = ((SDL2_Window*)window)->GL_CreateContext();

	GENESIS_ENGINE_ASSERT_ERROR((glewInit() == GLEW_OK), "Glew couldn't be initialized");

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	GENESIS_ENGINE_INFO("Using OpenGL Version: {}.{}", major, minor);

	if ((major > 4 || (major == 4 && minor >= 5)) || GLEW_ARB_clip_control)
	{
		//Matches Vulkan's clip space
		glClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE);
	}
	else
	{
		GENESIS_ENGINE_ERROR("glClipControl required, sorry!");
		exit(1);
	}
}

OpenglBackend::~OpenglBackend()
{
	this->window->GL_DeleteContext(this->opengl_context);
}

vector2U OpenglBackend::getScreenSize()
{
	return this->viewport_size;
}

void OpenglBackend::startFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	vector2U window_size = this->window->getWindowSize();

	if (window_size.y == 0) // Prevent A Divide By Zero By
	{
		window_size.y = 1;// Making Height Equal One
	}

	glViewport(0, 0, window_size.x, window_size.y);
	this->viewport_size = window_size;

	this->current_program = nullptr;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
}

void OpenglBackend::endFrame()
{
	this->window->GL_UpdateBuffer();
}

GLenum getVertexElementType(VertexElementType type)
{
	switch (type)
	{
	case Genesis::VertexElementType::float_1:
	case Genesis::VertexElementType::float_2:
	case Genesis::VertexElementType::float_3:
	case Genesis::VertexElementType::float_4:
		return GL_FLOAT;
	case Genesis::VertexElementType::unorm8_1:
	case Genesis::VertexElementType::unorm8_2:
	case Genesis::VertexElementType::unorm8_3:
	case Genesis::VertexElementType::unorm8_4:
		return GL_UNSIGNED_BYTE;
	case Genesis::VertexElementType::uint8_1:
	case Genesis::VertexElementType::uint8_2:
	case Genesis::VertexElementType::uint8_3:
	case Genesis::VertexElementType::uint8_4:
		return GL_BYTE;
	case Genesis::VertexElementType::uint16_1:
	case Genesis::VertexElementType::uint16_2:
	case Genesis::VertexElementType::uint16_3:
	case Genesis::VertexElementType::uint16_4:
		return GL_UNSIGNED_SHORT;
	case Genesis::VertexElementType::uint32_1:
	case Genesis::VertexElementType::uint32_2:
	case Genesis::VertexElementType::uint32_3:
	case Genesis::VertexElementType::uint32_4:
		return GL_UNSIGNED_INT;
	}

	return 0;
}

VertexBuffer OpenglBackend::createVertexBuffer(void* data, uint64_t data_size, const VertexInputDescriptionCreateInfo& vertex_description)
{
	OpenglVertexBuffer* vertex_buffer = new OpenglVertexBuffer();
	glGenVertexArrays(1, &vertex_buffer->vertex_array_object);
	glBindVertexArray(vertex_buffer->vertex_array_object);

	glGenBuffers(1, &vertex_buffer->vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_STATIC_DRAW);

	uint32_t total_size = 0;
	vector<size_t> offsets(vertex_description.input_elements_count);
	for (uint32_t i = 0; i < vertex_description.input_elements_count; i++)
	{
		offsets[i] = total_size;
		total_size += VertexElementTypeInfo::getInputElementSizeByte(vertex_description.input_elements[i]);
	}

	for (uint32_t i = 0; i < vertex_description.input_elements_count; i++)
	{
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(
			i,                  // attribute location
			VertexElementTypeInfo::getInputElementCount(vertex_description.input_elements[i]), // count
			getVertexElementType(vertex_description.input_elements[i]), // type
			GL_FALSE,           // normalized?
			total_size,     // stride
			(void*)offsets[i] // array buffer offset
		);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	for (uint32_t i = 0; i < vertex_description.input_elements_count; i++)
	{
		glEnableVertexAttribArray(i);
	}

	return vertex_buffer;
}

void OpenglBackend::destoryVertexBuffer(VertexBuffer buffer)
{
	OpenglVertexBuffer* vertex_buffer = (OpenglVertexBuffer*)buffer;
	glDeleteVertexArrays(1, &vertex_buffer->vertex_array_object);
	glDeleteBuffers(1, &vertex_buffer->vertex_buffer);
	delete vertex_buffer;
}

IndexBuffer OpenglBackend::createIndexBuffer(void* data, uint64_t data_size, IndexType type)
{
	OpenglIndexBuffer* index_buffer = new OpenglIndexBuffer();

	glGenBuffers(1, &index_buffer->index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, data_size, data, GL_STATIC_DRAW);

	index_buffer->type = type;

	return (IndexBuffer)index_buffer;
}

void OpenglBackend::destoryIndexBuffer(IndexBuffer buffer)
{
	OpenglIndexBuffer* index_buffer = (OpenglIndexBuffer*)buffer;
	glDeleteBuffers(1, &index_buffer->index_buffer);
	delete index_buffer;
}

Texture2D OpenglBackend::createTexture(TextureFormat format, vector2U size, void* data, uint64_t data_size)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	GLenum gl_format;
	switch (format)
	{
	case Genesis::TextureFormat::R:
		gl_format = GL_R;
		break;
	case Genesis::TextureFormat::RG:
		gl_format = GL_RG;
		break;
	case Genesis::TextureFormat::RGB:
		gl_format = GL_RGB;
		break;
	case Genesis::TextureFormat::RGBA:
		gl_format = GL_RGBA;
		break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, gl_format, size.x, size.y, 0, gl_format, GL_UNSIGNED_BYTE, data);

	return (Texture2D)texture;
}

void OpenglBackend::destoryTexture(Texture2D texture)
{
	glDeleteTextures(1, &((GLuint)texture));
}

ShaderProgram OpenglBackend::createShaderProgram(const char* vert_data, uint32_t vert_size, const char* frag_data, uint32_t frag_size)
{
	return (ShaderProgram)new OpenglShaderProgram(vert_data, vert_size, frag_data, frag_size);
}

void OpenglBackend::destoryShaderProgram(ShaderProgram program)
{
	delete (OpenglShaderProgram*)program;
}

void OpenglBackend::bindShaderProgram(ShaderProgram program)
{
	this->current_program = (OpenglShaderProgram*)program;

	if (this->current_program != nullptr)
	{
		glUseProgram(current_program->getProgramID());
	}
	else
	{
		glUseProgram(0);
	}
}

void OpenglBackend::setUniform1i(const string& name, const int32_t& value)
{
	GENESIS_ENGINE_ASSERT_ERROR((this->current_program != nullptr), "Shader Not Bound");
	glUniform1i(current_program->getUniformLocation(name), value);
}

void OpenglBackend::setUniform1u(const string& name, const uint32_t& value)
{
	GENESIS_ENGINE_ASSERT_ERROR((this->current_program != nullptr), "Shader Not Bound");
	glUniform1ui(current_program->getUniformLocation(name), value);
}

void OpenglBackend::setUniform2u(const string& name, const vector2U& value)
{
	GENESIS_ENGINE_ASSERT_ERROR((this->current_program != nullptr), "Shader Not Bound");
	glUniform2ui(current_program->getUniformLocation(name), value.x, value.y);
}

void OpenglBackend::setUniform3u(const string& name, const vector3U& value)
{
	GENESIS_ENGINE_ASSERT_ERROR((this->current_program != nullptr), "Shader Not Bound");
	glUniform3ui(current_program->getUniformLocation(name), value.x, value.y, value.z);
}

void OpenglBackend::setUniform4u(const string& name, const vector4U& value)
{
	GENESIS_ENGINE_ASSERT_ERROR((this->current_program != nullptr), "Shader Not Bound");
	glUniform4ui(current_program->getUniformLocation(name), value.x, value.y, value.z, value.w);
}

void OpenglBackend::setUniform1f(const string& name, const float& value)
{
	GENESIS_ENGINE_ASSERT_ERROR((this->current_program != nullptr), "Shader Not Bound");
	glUniform1f(current_program->getUniformLocation(name), value);
}

void OpenglBackend::setUniform2f(const string& name, const vector2F& value)
{
	GENESIS_ENGINE_ASSERT_ERROR((this->current_program != nullptr), "Shader Not Bound");
	glUniform2f(current_program->getUniformLocation(name), value.x, value.y);
}

void OpenglBackend::setUniform3f(const string& name, const vector3F& value)
{
	GENESIS_ENGINE_ASSERT_ERROR((this->current_program != nullptr), "Shader Not Bound");
	glUniform3f(current_program->getUniformLocation(name), value.x, value.y, value.z);
}

void OpenglBackend::setUniform4f(const string& name, const vector4F& value)
{
	GENESIS_ENGINE_ASSERT_ERROR((this->current_program != nullptr), "Shader Not Bound");
	glUniform4f(current_program->getUniformLocation(name), value.x, value.y, value.z, value.w);
}

void OpenglBackend::setUniformMat3f(const string& name, const matrix3F& value)
{
	GENESIS_ENGINE_ASSERT_ERROR((this->current_program != nullptr), "Shader Not Bound");
	glUniformMatrix3fv(current_program->getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}

void OpenglBackend::setUniformMat4f(const string& name, const matrix4F& value)
{
	GENESIS_ENGINE_ASSERT_ERROR((this->current_program != nullptr), "Shader Not Bound");
	glUniformMatrix4fv(current_program->getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}

void OpenglBackend::setUniformTexture(const string& name, const uint32_t texture_slot, const Texture2D& value)
{
	GENESIS_ENGINE_ASSERT_ERROR((this->current_program != nullptr), "Shader Not Bound");

	glActiveTexture(GL_TEXTURE0 + texture_slot);
	glBindTexture(GL_TEXTURE_2D, value);
	glUniform1i(current_program->getUniformLocation(name), texture_slot);
}

void OpenglBackend::draw(VertexBuffer vertex_buffer, IndexBuffer index_buffer, uint32_t triangle_count)
{
	OpenglVertexBuffer* vertex = (OpenglVertexBuffer*)vertex_buffer;
	OpenglIndexBuffer* index = (OpenglIndexBuffer*)index_buffer;

	glBindVertexArray(vertex->vertex_array_object);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index->index_buffer);

	if (index->type == IndexType::uint32)
	{
		glDrawElements(GL_TRIANGLES, triangle_count, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawElements(GL_TRIANGLES, triangle_count, GL_UNSIGNED_SHORT, 0);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void OpenglBackend::TEMP_enableAlphaBlending(bool enable)
{
	if (enable)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glDepthMask(false);
		glDepthFunc(GL_EQUAL);
	}
	else
	{
		glDepthFunc(GL_LESS);
		glDepthMask(true);
		glDisable(GL_BLEND);
	}
}
