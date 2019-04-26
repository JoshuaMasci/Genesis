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

		bool operator==(const TexturedVertex& other) const
		{
			return pos == other.pos && normal == other.normal && uv == other.uv;
		}
	};

	class TexturedMesh
	{
	public:
		TexturedMesh(vector<TexturedVertex>& vertices, vector<uint32_t>& indices);
		virtual ~TexturedMesh();

		virtual void draw();

		static TexturedMesh* loadObj(string file_name);

	protected:
		GLuint vbo;
		GLuint ibo;

		int size;
	};
}