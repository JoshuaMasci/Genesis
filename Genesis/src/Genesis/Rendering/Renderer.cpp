#include "Renderer.hpp"

#include "Genesis/Application.hpp"

using namespace Genesis;

Renderer::Renderer(RenderingBackend* backend)
{
	this->backend = backend;
}

Renderer::~Renderer()
{
	for (auto mesh : this->loaded_meshes)
	{
		delete mesh.second.vertices;
		delete mesh.second.indices;
	}
}

void Renderer::drawFrame(EntityRegistry& EntityRegistry)
{
	float fov = 1.0f / tan(glm::radians(75.0f) / 2.0f);
	float aspect = 2560.0f / 1440.0f;
	matrix4F view = glm::lookAt(vector3F(0.0f, 2.0f, -4.0f), vector3F(0.0f), vector3F(0.0f, 1.0f, 0.0f));
	matrix4F proj = glm::infinitePerspective(fov, aspect, 0.1f);
	proj[1][1] *= -1; //Need to apply this because vulkan flips the y-axis and that's not what I need

	matrix4F mv = proj * view;

	bool result = this->backend->beginFrame();

	if (result)
	{
		auto view = EntityRegistry.view<Model, WorldTransform>();

		for (auto entity : view)
		{
			auto &model = view.get<Model>(entity);
			auto &transform = view.get<WorldTransform>(entity);
			
			Mesh* mesh = &this->loaded_meshes[model.mesh];

			matrix4F translation = glm::translate(matrix4F(1.0F), (vector3F)transform.current_transform.getPosition());
			matrix4F orientation = glm::toMat4((quaternionF)transform.current_transform.getOrientation());
			matrix4F mvp = mv * (translation * orientation);
			this->backend->drawMeshScreen(0, mesh->vertices, mesh->indices, mesh->indices_count, mvp);
		}

		this->backend->endFrame();
	}
}

//Temp resource stuff
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

void Renderer::loadMesh(string mesh_file)
{
	if (this->loaded_meshes.find(mesh_file) == this->loaded_meshes.end())
	{
		tinyobj::attrib_t attrib;
		vector<tinyobj::shape_t> shapes;
		vector<tinyobj::material_t> materials;
		string warn, err;

		if (tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, mesh_file.c_str()))
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

			Mesh* mesh = &this->loaded_meshes[mesh_file];

			mesh->vertices = this->backend->createBuffer(sizeof(TexturedVertex) * vertices.size(), BufferType::Vertex, MemoryUsage::GPU_Only);
			mesh->vertices->fill(vertices.data(), sizeof(TexturedVertex) * vertices.size());

			mesh->indices = this->backend->createBuffer(sizeof(uint32_t) * indices.size(), BufferType::Index, MemoryUsage::GPU_Only);
			mesh->indices->fill(indices.data(), sizeof(uint32_t) * indices.size());

			mesh->indices_count = (uint32_t)indices.size();
		}
		else
		{
			printf("Error: Can't load Mesh: %s\n", mesh_file.c_str());
		}
	}
	else
	{
		printf("Error: Already loaded Mesh: %s\n", mesh_file.c_str());
	}
}

void Renderer::loadTexture(string texture_file)
{
}
