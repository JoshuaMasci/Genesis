#include "ShaderProgram.hpp"

using namespace Genesis;

ShaderProgram::ShaderProgram()
{
	//Do nothing until real constructor is called.
	this->programID = 0;
}

ShaderProgram::ShaderProgram(string VertexShader, string FragmentShader)
{
	programID = glCreateProgram();
	vertexID = buildShader(VertexShader, GL_VERTEX_SHADER);
	fragmentID = buildShader(FragmentShader, GL_FRAGMENT_SHADER);
	if (vertexID == 0 || fragmentID == 0)
	{
		//Logger::getInstance()->logError("Shaders did not compile\n");
	}

	glAttachShader(programID, vertexID);
	glAttachShader(programID, fragmentID);

	glLinkProgram(programID);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Check the program
	glGetProgramiv(programID, GL_LINK_STATUS, &Result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		if (ProgramErrorMessage.size() > 2)
		{
			//Logger::getInstance()->logError("shader: %s\n", &ProgramErrorMessage[0]);
		}
	}
	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);
}

void ShaderProgram::setActiveProgram()
{
	glUseProgram(this->programID);
}

ShaderProgram::~ShaderProgram()
{
	glDetachShader(programID, vertexID);
	glDetachShader(programID, fragmentID);
	glDeleteProgram(programID);
}

void ShaderProgram::setUniform(string name, const int& value)
{
	glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

void ShaderProgram::setUniform(string name, const unsigned int& value)
{
	glUniform1ui(glGetUniformLocation(programID, name.c_str()), value);
}

void ShaderProgram::setUniform(string name, const float& value)
{
	glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

void ShaderProgram::setUniform(string name, const matrix4F& matrix)
{
	glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
}

void ShaderProgram::setUniform(string name, const matrix3F& matrix)
{
	glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
}

void ShaderProgram::setUniform(string name, const vector3F& vec)
{
	glUniform3f(glGetUniformLocation(programID, name.c_str()), vec.x, vec.y, vec.z);
}

void ShaderProgram::setUniform(string name, const vector2F& vec)
{
	glUniform2f(glGetUniformLocation(programID, name.c_str()), vec.x, vec.y);
}

void ShaderProgram::setUniform(string name, const quaternionF& quat)
{
	glUniform4f(glGetUniformLocation(programID, name.c_str()), quat.w, quat.x, quat.y, quat.z);
}

GLuint ShaderProgram::buildShader(string location, GLuint type)
{
	GLuint ShaderID = glCreateShader(type);

	// Read the  Shader code from the file
	std::string ShaderCode;

	ShaderCode += loadShaderFile(location);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile  Shader
	char const * SourcePointer = ShaderCode.c_str();
	glShaderSource(ShaderID, 1, &SourcePointer, NULL);
	glCompileShader(ShaderID);

	// Check  Shader
	glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> ShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(ShaderID, InfoLogLength, NULL, &ShaderErrorMessage[0]);
		if (ShaderErrorMessage.size() > 2)
		{
			//Logger::getInstance()->logError("Shader: %s\n", &ShaderErrorMessage[0]);
		}
	}

	return ShaderID;
}

string ShaderProgram::loadShaderFile(string location)
{
	const char * file_path = location.c_str();

	// Read the  Shader code from the file
	std::string ShaderCode;
	std::ifstream ShaderStream(file_path, std::ios::in);

	if (ShaderStream.is_open())
	{
		std::string Line = "";
		while (getline(ShaderStream, Line))
		{
			const string INCLUDE_DIRECTIVE = "#include \"";
			//If the line doesnt have an include statement
			//Add it to the Shader Code
			if (Line.find(INCLUDE_DIRECTIVE) == std::string::npos)
			{
				ShaderCode += "\n" + Line;
			}
			//If it has a include statement
			else
			{
				size_t includeLength = INCLUDE_DIRECTIVE.length();
				//SubString from the first " to the end "\n
				string filePath = Line.substr(includeLength, Line.length() - includeLength - 1);
				ShaderCode += loadShaderFile(filePath);
			}
		}
		ShaderStream.close();
	}
	else
	{
		//Logger::getInstance()->logError("%s Shader File not found\n", file_path);
		ShaderCode = "";
	}


	return ShaderCode;
}