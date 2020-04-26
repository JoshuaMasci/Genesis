#include "OpenglShaderProgram.hpp"

#include "Genesis/Debug/Log.hpp"
#include "Genesis/Debug/Assert.hpp"

using namespace Genesis;
using namespace Genesis::Opengl;

OpenglShaderProgram::OpenglShaderProgram(const char* vert_data, uint32_t vert_size, const char* frag_data, uint32_t frag_size)
{
	this->program_id = glCreateProgram();
	GLuint vertex_id = buildShader(vert_data, vert_size, GL_VERTEX_SHADER);
	GLuint fragment_id = buildShader(frag_data, frag_size, GL_FRAGMENT_SHADER);
	if (vertex_id == 0 || fragment_id == 0)
	{
		GENESIS_ENGINE_ERROR("Shaders did not compile\n");
	}

	glAttachShader(this->program_id, vertex_id);
	glAttachShader(this->program_id, fragment_id);

	glLinkProgram(this->program_id);

	GLint result = GL_FALSE;
	int info_log_length;

	// Check the program
	glGetProgramiv(this->program_id, GL_LINK_STATUS, &result);
	glGetProgramiv(this->program_id, GL_INFO_LOG_LENGTH, &info_log_length);
	if (info_log_length > 0)
	{
		vector<char> program_error_message(info_log_length + 1);
		glGetProgramInfoLog(this->program_id, info_log_length, NULL, &program_error_message[0]);
		if (program_error_message.size() > 2)
		{
			GENESIS_ENGINE_ERROR("Shader Error: {}", &program_error_message[0]);
		}
	}
	glDeleteShader(vertex_id);
	glDeleteShader(fragment_id);
}

OpenglShaderProgram::~OpenglShaderProgram()
{
	glDeleteProgram(this->program_id);
}

GLuint OpenglShaderProgram::buildShader(const char* data, uint32_t size, GLuint type)
{
	GLuint shader_id = glCreateShader(type);

	GLint result = GL_FALSE;
	int info_log_length;

	const GLint value = size;

	// Compile  Shader
	glShaderSource(shader_id, 1, &data, &value);
	glCompileShader(shader_id);

	// Check  Shader
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
	if (info_log_length > 0)
	{
		vector<char> shader_error_message(info_log_length + 1);
		glGetShaderInfoLog(shader_id, info_log_length, NULL, &shader_error_message[0]);
		if (shader_error_message.size() > 2)
		{
			GENESIS_ENGINE_ERROR("Shader Error: {}", &shader_error_message[0]);
		}
	}

	return shader_id;
}

GLint OpenglShaderProgram::getUniformLocation(const string& name)
{
	if (!has_value(this->uniform_locations, name))
	{
		GLint location = glGetUniformLocation(this->program_id, name.c_str());
		if (location == -1)
		{
			GENESIS_ENGINE_WARN("Uniform Location doesn't exist: {}", name);
		}
		this->uniform_locations[name] = location;
	}

	return this->uniform_locations[name];
}
