#pragma once

#include <Opengl_Include.hpp>
#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	namespace Opengl
	{
		class OpenglShaderProgram
		{
		public:
			OpenglShaderProgram(const char* vert_data, uint32_t vert_size, const char* frag_data, uint32_t frag_size);
			~OpenglShaderProgram();

			GLuint getProgramID() { return this->program_id; };
			GLuint getUniformLocation(const string& name);

		protected:
			GLuint program_id;

			GLuint buildShader(const char* data, uint32_t size, GLuint type);

			map<string, GLuint> uniform_locations;
		};
	}
}