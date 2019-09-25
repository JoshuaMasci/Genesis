#include "Renderer.hpp"

#include "Genesis/Application.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/PipelineSettings.hpp"
#include "Genesis/WorldTransform.hpp"

#include <fstream>

using namespace Genesis;

Renderer::Renderer(RenderingBackend* backend)
{
	this->backend = backend;
}

Renderer::~Renderer()
{
	for (auto mesh : this->loaded_meshes)
	{
		this->backend->destroyVertexBuffer(mesh.second.vertices);
		this->backend->destroyIndexBuffer(mesh.second.indices);
	}

	for (auto texture : this->loaded_textures)
	{
		this->backend->destroyTexture(texture.second);
	}

	for (auto shader : this->loaded_shaders)
	{
		this->backend->destroyShader(shader.second);
	}
}

void Renderer::drawFrame(EntityRegistry& entity_registry, EntityId camera_entity)
{
	if (!entity_registry.has<WorldTransform>(camera_entity) || !entity_registry.has<Camera>(camera_entity))
	{
		//No camera
		return;
	}

	bool result = this->backend->beginFrame();

	if (!result)
	{
		return;
	}

	CommandBuffer* command_buffer = this->backend->getScreenCommandBuffer();

	if (command_buffer == nullptr)
	{
		return;
	}

	vector2I zero = {0,0};
	vector2U screen_size = this->backend->getScreenSize();
	command_buffer->setScissor(zero, screen_size);
	command_buffer->setPipelineSettings(PipelineSettings());

	float aspect_ratio = ((float)screen_size.x) / ((float)screen_size.y);

	auto& camera = entity_registry.get<Camera>(camera_entity);
	auto& transform = entity_registry.get<WorldTransform>(camera_entity);
	Transform& current = transform.current_transform;

	matrix4F view = glm::lookAt(current.getPosition(), current.getPosition() + current.getForward(), current.getUp());
	matrix4F proj = this->backend->getPerspectiveMatrix(&camera, aspect_ratio);
	matrix4F mv = proj * view;

	auto entity_model = entity_registry.view<Model, WorldTransform>();
	for (auto entity : entity_model)
	{
		auto &model = entity_model.get<Model>(entity);
		auto &transform = entity_model.get<WorldTransform>(entity);

		matrix4F translation = glm::translate(matrix4F(1.0F), (vector3F)(transform.current_transform.getPosition()));
		matrix4F orientation = glm::toMat4((quaternionF)transform.current_transform.getOrientation());
		matrix4F mvp = mv * (translation * orientation);

		command_buffer->setShader(model.shader);
		command_buffer->setUniformTexture("albedo_texture", model.texture);
		command_buffer->setUniformMat4("matrices.mvp", mvp);
		command_buffer->drawIndexed(model.vertices, model.indices);
	}
}

void Renderer::endFrame()
{
	this->backend->endFrame();
	vector<ViewHandle> sub_views;
	this->backend->submitFrame(sub_views);
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
						vector2F uv = vector2F(attrib.texcoords[2 * idx.texcoord_index + 0], -attrib.texcoords[2 * idx.texcoord_index + 1]);

						TexturedVertex vertex = { position, normal, uv };

						vertices.push_back(vertex);
						indices.push_back((uint32_t)indices.size());
					}
					index_offset += fv;
				}
			}

			VertexInputDescription vertex_description
			({
			{"in_position", VertexElementType::float_3},
			{"in_normal", VertexElementType::float_3},
			{"in_uv", VertexElementType::float_2}
			});

			Mesh* mesh = &this->loaded_meshes[mesh_file];

			mesh->vertices = this->backend->createVertexBuffer(vertices.data(), sizeof(TexturedVertex) * vertices.size(), vertex_description);
			mesh->indices = this->backend->createIndexBuffer(indices.data(), (uint32_t)indices.size(), IndexType::uint32);
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

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image.h>

void Renderer::loadTexture(string texture_file)
{
	if (this->loaded_textures.find(texture_file) == this->loaded_textures.end())
	{
		int width, height, tex_channels;
		uint8_t* data = stbi_load((texture_file).c_str(), &width, &height, &tex_channels, STBI_rgb_alpha);
		uint64_t data_size = width * height * STBI_rgb_alpha;

		if (data == NULL)
		{
			printf("Error: Can't load Texture: %s\n", texture_file.c_str());
			return;
		}

		TextureHandle index = this->backend->createTexture(vector2U((uint32_t)width, (uint32_t)height), (void*)data, data_size);
		stbi_image_free(data);
		this->loaded_textures[texture_file] = index;
	}
	else
	{
		printf("Error: Already loaded Texture: %s\n", texture_file.c_str());
	}
}

void Renderer::loadShader(string shader_file_base)
{
	if (this->loaded_shaders.find(shader_file_base) == this->loaded_shaders.end())
	{
		string vert_data = "";
		string frag_data = "";

		std::ifstream vert_file(shader_file_base + ".vert.spv", std::ios::ate | std::ios::binary);
		if (vert_file.is_open())
		{
			size_t fileSize = (size_t)vert_file.tellg();
			vert_file.seekg(0);
			vert_data.resize(fileSize);
			vert_file.read(vert_data.data(), vert_data.size());
			vert_file.close();
		}

		std::ifstream frag_file(shader_file_base + ".frag.spv", std::ios::ate | std::ios::binary);
		if (frag_file.is_open())
		{
			size_t fileSize = (size_t)frag_file.tellg();
			frag_file.seekg(0);
			frag_data.resize(fileSize);
			frag_file.read(frag_data.data(), frag_data.size());
			frag_file.close();
		}

		this->loaded_shaders[shader_file_base] = this->backend->createShader(vert_data, frag_data);
	}
	else
	{
		printf("Error: Already loaded Shader: %s\n", shader_file_base.c_str());
	}
}

Model Renderer::createModel(string mesh, string texture, string shader)
{
	Model model = {};
	
	//TODO keep count of using
	if (this->loaded_meshes.find(mesh) == this->loaded_meshes.end())
	{
		this->loadMesh(mesh);
	}

	if (this->loaded_textures.find(texture) == this->loaded_textures.end())
	{
		this->loadTexture(texture);
	}

	if (this->loaded_shaders.find(shader) == this->loaded_shaders.end())
	{
		this->loadShader(shader);
	}

	model.vertices = this->loaded_meshes[mesh].vertices;
	model.indices = this->loaded_meshes[mesh].indices;
	model.texture = this->loaded_textures[texture];
	model.shader = this->loaded_shaders[shader];
	return model;
}
