#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Graphics/OpenGL/OpenGL.hpp"
#include "Genesis/Graphics/OpenGL/ShaderProgram.hpp"

namespace Genesis
{
	struct TexturedVertex
	{
		vector3F pos;
		vector3F normal;
		vector2F uv;
	};

	class TexturedMesh
	{
	public:
		TexturedMesh(vector<TexturedVertex>& vertices, vector<unsigned int>& indices);
		virtual ~TexturedMesh();

		virtual void draw(ShaderProgram* program);

		static TexturedMesh* loadObj(string file_name);

	protected:
		GLuint vbo;
		GLuint ibo;

		int size;
	};
}