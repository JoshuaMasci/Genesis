#include "TexturedMesh.hpp"

using namespace Genesis;

TexturedMesh::TexturedMesh(vector<TexturedVertex>& vertices, vector<unsigned int>& indices)
{
	this->size= (int)indices.size();

	//GenBuffers
	glGenBuffers(1, &this->vbo);
	glGenBuffers(1, &this->ibo);

	//Adds the data to the buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TexturedVertex), &vertices[0], GL_STATIC_DRAW);

	//Adds the indices to the buffer.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->size * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}

TexturedMesh::~TexturedMesh()
{
	glDeleteBuffers(1, &this->vbo);
	glDeleteBuffers(1, &this->ibo);
}

void TexturedMesh::draw(ShaderProgram* program)
{
	if (this->size == 0)
	{
		return;
	}

	//Enable Attributes
	glEnableVertexAttribArray(0); //Position
	glEnableVertexAttribArray(1); //Normal
	glEnableVertexAttribArray(2); //UV

	//Bind the buffer to begin drawing
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

	//Position
	glVertexAttribPointer(
		0,                  // Position attribute location
		3,					// size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		sizeof(TexturedVertex),     // stride
		(void*)0            // array buffer offset
		);

	//Normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(TexturedVertex), (void*)offsetof(TexturedVertex, normal));

	//UV
	glVertexAttribPointer(2, 2, GL_FLOAT,GL_FALSE, sizeof(TexturedVertex), (void*)offsetof(TexturedVertex, uv));

	//Draw the mesh
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);
	glDrawElements(GL_TRIANGLES, this->size, GL_UNSIGNED_INT, 0);

	//Disable Attributes
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

TexturedMesh* TexturedMesh::loadObj(string file_name)
{
	TexturedMesh* texture_mesh = nullptr;

	return texture_mesh;
}