#pragma once

#include <Opengl_Include.hpp>

namespace genesis
{
	namespace Opengl
	{
		struct ShaderStageInfo
		{
			const char* data;
			uint32_t size;
			GLuint type;
		};

		class OpenglShaderProgram
		{
		public:
			OpenglShaderProgram(const char* vert_data, uint32_t vert_size, const char* frag_data, uint32_t frag_size);
			OpenglShaderProgram(const ShaderStageInfo* stages, uint32_t size);

			~OpenglShaderProgram();

			GLuint getProgramID() { return this->program_id; };
			GLint getUniformLocation(const string& name);

		protected:
			GLuint program_id;

			GLuint buildShader(const ShaderStageInfo& info);

			flat_hash_map<string, GLint> uniform_locations;
		};
	}
}