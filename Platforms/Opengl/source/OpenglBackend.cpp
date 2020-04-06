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
		glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
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

void OpenglBackend::startFrame()
{
	vector2U window_size = this->window->getWindowSize();

	if (window_size.y == 0) // Prevent A Divide By Zero By
	{
		window_size.y = 1;// Making Height Equal One
	}

	glViewport(0, 0, window_size.x, window_size.y);

	this->current_program = nullptr;
}

void OpenglBackend::endFrame()
{
	this->window->GL_UpdateBuffer();
}

Buffer OpenglBackend::createBuffer(BufferType type, void* data, uint64_t data_size)
{
	GLuint buffer;
	glGenBuffers(1, &buffer);

	if (type == BufferType::Vertex_Buffer)
	{
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_STATIC_DRAW);
	}
	else if (type == BufferType::Index_Buffer)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data_size, data, GL_STATIC_DRAW);
	}

	return (Buffer)buffer;
}

void OpenglBackend::destoryBuffer(Buffer buffer)
{
	GLuint gl_buffer = (GLuint)buffer;
	glDeleteBuffers(1, &gl_buffer);
}

Texture2D OpenglBackend::createTexture(TextureFormat format, vector2F size, void * data, uint64_t data_size)
{
	return Texture2D();
}

void OpenglBackend::destoryTexture(Texture2D texture)
{
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

void OpenglBackend::draw(Buffer vertex_buffer, Buffer index_buffer, uint32_t triangle_count)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glEnableVertexAttribArray(0); //Position
	glEnableVertexAttribArray(1); //Color

	//Position
	glVertexAttribPointer(
		0,                  // Position attribute location
		3,					// size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		sizeof(vector3F) * 2,     // stride
		(void*)0            // array buffer offset
	);

	//Color
	glVertexAttribPointer(
		1,                  // Color attribute location
		3,					// size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		sizeof(vector3F) * 2,     // stride
		(void*) sizeof(vector3F) // array buffer offset
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glDrawElements(GL_TRIANGLES, triangle_count, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}
