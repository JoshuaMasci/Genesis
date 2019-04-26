#pragma once

#include "Genesis/Graphics/OpenGL/OpenGL.hpp"
#include "Genesis/Core/VectorTypes.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

namespace Genesis
{
	class ShaderProgram
	{
	public:
		GLuint programID = 0;
		ShaderProgram();
		ShaderProgram(string VertexShaderLoc, string FragmentShaderLoc);
		~ShaderProgram();
		void setActiveProgram();

		void setUniform(string name, const int& value);
		void setUniform(string name, const unsigned int& value);
		void setUniform(string name, const float& value);
		void setUniform(string name, const matrix4F& matrix);
		void setUniform(string name, const matrix3F& matrix);
		void setUniform(string name, const vector3F& vec);
		void setUniform(string name, const vector2F& vec);
		void setUniform(string name, const quaternionF& quat);

	private:
		GLuint vertexID, fragmentID;
		GLuint buildShader(string location, GLuint type);
		string loadShaderFile(string location);
	};
}