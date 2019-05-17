#include "TexturedMesh.hpp"

using namespace Genesis;

TexturedMesh::TexturedMesh(vector<TexturedVertex>& vertices, vector<uint32_t>& indices)
{
	this->size = (int)indices.size();

	//GenBuffers
	glGenBuffers(1, &this->vbo);
	glGenBuffers(1, &this->ibo);

	//Adds the data to the buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TexturedVertex), &vertices[0], GL_STATIC_DRAW);

	//Adds the indices to the buffer.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->size * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);
}

TexturedMesh::~TexturedMesh()
{
	glDeleteBuffers(1, &this->vbo);
	glDeleteBuffers(1, &this->ibo);
}

void TexturedMesh::draw()
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

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

TexturedMesh* TexturedMesh::loadObj(string file_name)
{
	TexturedMesh* texture_mesh = nullptr;

	tinyobj::attrib_t attrib;
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;
	string warn, err;

	if (tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file_name.c_str()))
	{
		vector<TexturedVertex> vertices;
		vector<uint32_t> indices;

		for (const auto& shape : shapes)
		{
			size_t index_offset = 0;
			for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
			{
				int fv = shape.mesh.num_face_vertices[f];

				// Loop over vertices in the face
				for (size_t v = 0; v < fv; v++)
				{
					// access to vertex
					tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
					vector3F position = vector3F(attrib.vertices[3 * idx.vertex_index + 0], attrib.vertices[3 * idx.vertex_index + 1], attrib.vertices[3 * idx.vertex_index + 2]);
					vector3F normal = vector3F(attrib.normals[3 * idx.normal_index + 0], attrib.normals[3 * idx.normal_index + 1], attrib.normals[3 * idx.normal_index + 2]);
					vector2F uv = vector2F(attrib.texcoords[2 * idx.texcoord_index + 0], attrib.texcoords[2 * idx.texcoord_index + 1]);

					TexturedVertex vertex = { position, normal, uv };


					vertices.push_back(vertex);
					indices.push_back((uint32_t)indices.size());
				}
				index_offset += fv;
			}
		}

		texture_mesh = new TexturedMesh(vertices, indices);
	}
	else
	{
		printf("Error: Can't load Mesh: %s\n", file_name.c_str());
	}

	return texture_mesh;
}