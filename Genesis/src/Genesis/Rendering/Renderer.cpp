#include "Renderer.hpp"

#include "Genesis/Application.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/PipelineSettings.hpp"

#include <fstream>

using namespace Genesis;

Renderer::Renderer(RenderingBackend* backend)
{
	this->backend = backend;


	{
		string vert_data = "";
		string frag_data = "";

		{
			string file_name = "resources/shaders/Vulkan/texture.vert.spv";
			std::ifstream file(file_name, std::ios::ate | std::ios::binary);
			size_t fileSize = (size_t)file.tellg();
			file.seekg(0);
			vert_data.resize(fileSize);
			file.read(vert_data.data(), vert_data.size());
			file.close();
		}

		{
			string file_name = "resources/shaders/Vulkan/texture.frag.spv";
			std::ifstream file(file_name, std::ios::ate | std::ios::binary);
			size_t fileSize = (size_t)file.tellg();
			file.seekg(0);
			frag_data.resize(fileSize);
			file.read(frag_data.data(), frag_data.size());
			file.close();
		}

		ShaderHandle shader = this->backend->createShader(vert_data, frag_data);
	}
}

Renderer::~Renderer()
{
	for (auto textures : this->loaded_textures)
	{
		this->backend->destroyTexture(textures.second);
	}

	for (auto mesh : this->loaded_meshes)
	{
		this->backend->destroyBuffer(mesh.second.vertices);
		this->backend->destroyBuffer(mesh.second.indices);
	}
}

void Renderer::drawFrame(EntityRegistry& entity_registry, EntityId camera_entity)
{
	PipelineSettings base_pipeline = {};

	if (!entity_registry.has<WorldTransform>(camera_entity) || !entity_registry.has<Camera>(camera_entity))
	{
		//No camera
		return;
	}

	bool result = this->backend->beginFrame();

	if (result)
	{
		vector2U screen_size = this->backend->getScreenSize();
		float aspect_ratio = ((float)screen_size.x) / ((float)screen_size.y);

		auto& camera = entity_registry.get<Camera>(camera_entity);
		auto& transform = entity_registry.get<WorldTransform>(camera_entity);
		Transform& current = transform.current_transform;

		matrix4F view = glm::lookAt(current.getPosition(), current.getPosition() + current.getForward(), current.getUp());
		matrix4F proj = this->backend->getPerspectiveMatrix(&camera, aspect_ratio);
		matrix4F mv = proj * view;

		vector<ViewHandle> sub_views;

		auto entity_view = entity_registry.view<ViewModel, WorldTransform>();
		for (auto entity : entity_view)
		{
			auto &view_model = entity_view.get<ViewModel>(entity);
			if (view_model.view != nullptr)
			{
				this->drawView(entity_registry, view_model.camera_entity, view_model.view);
				sub_views.push_back(view_model.view);
			}

			Mesh* mesh = &this->loaded_meshes[view_model.mesh];
			auto &transform = entity_view.get<WorldTransform>(entity);

			matrix4F translation = glm::translate(matrix4F(1.0F), (vector3F)(transform.current_transform.getPosition()));
			matrix4F orientation = glm::toMat4((quaternionF)transform.current_transform.getOrientation());
			matrix4F mvp = mv * (translation * orientation);

			this->backend->drawMeshScreenViewTextured(0, mesh->vertices, mesh->indices, mesh->indices_count, view_model.view, mvp);
		}

		auto entity_model = entity_registry.view<Model, WorldTransform>();
		for (auto entity : entity_model)
		{
			auto &model = entity_model.get<Model>(entity);
			auto &transform = entity_model.get<WorldTransform>(entity);
			
			Mesh* mesh = &this->loaded_meshes[model.mesh];
			TextureHandle texture = this->loaded_textures[model.texture];

			matrix4F translation = glm::translate(matrix4F(1.0F), (vector3F)(transform.current_transform.getPosition()));
			matrix4F orientation = glm::toMat4((quaternionF)transform.current_transform.getOrientation());
			matrix4F mvp = mv * (translation * orientation);
			this->backend->drawMeshScreen(0, mesh->vertices, mesh->indices, mesh->indices_count, texture, mvp);
		}

		this->backend->endFrame();

		this->backend->submitFrame(sub_views);
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
						vector2F uv = vector2F(attrib.texcoords[2 * idx.texcoord_index + 0], -attrib.texcoords[2 * idx.texcoord_index + 1]);

						TexturedVertex vertex = { position, normal, uv };

						vertices.push_back(vertex);
						indices.push_back((uint32_t)indices.size());
					}
					index_offset += fv;
				}
			}

			Mesh* mesh = &this->loaded_meshes[mesh_file];

			mesh->vertices = this->backend->createBuffer(sizeof(TexturedVertex) * vertices.size(), BufferType::Vertex, MemoryUsage::GPU_Only);
			this->backend->fillBuffer(mesh->vertices, vertices.data(), sizeof(TexturedVertex) * vertices.size());

			mesh->indices = this->backend->createBuffer(sizeof(uint32_t) * indices.size(), BufferType::Index, MemoryUsage::GPU_Only);
			this->backend->fillBuffer(mesh->indices, indices.data(), sizeof(uint32_t) * indices.size());

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

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image.h>

void Renderer::loadTexture(string texture_file)
{
	if (this->loaded_textures.find(texture_file) == this->loaded_textures.end())
	{

		int width, height, tex_channels;
		unsigned char* data = stbi_load((texture_file).c_str(), &width, &height, &tex_channels, STBI_rgb_alpha);
		uint64_t data_size = width * height * STBI_rgb_alpha;

		if (data == NULL)
		{
			printf("Error: Can't load Texture: %s\n", texture_file.c_str());
			return;
		}

		TextureHandle index = this->backend->createTexture(vector2U((uint32_t)width, (uint32_t)height));
		this->backend->fillTexture(index, (void*)data, data_size);
		stbi_image_free(data);

		this->loaded_textures[texture_file] = index;
	}
	else
	{
		printf("Error: Already loaded Texture: %s\n", texture_file.c_str());
	}
}

void Renderer::drawView(EntityRegistry & entity_registry, EntityId camera_entity, ViewHandle view_handle)
{
	this->backend->startView(view_handle);

	auto& camera = entity_registry.get<Camera>(camera_entity);
	auto& transform = entity_registry.get<WorldTransform>(camera_entity);
	Transform& current = transform.current_transform;

	matrix4F view = glm::lookAt(current.getPosition(), current.getPosition() + current.getForward(), current.getUp());
	matrix4F proj = this->backend->getPerspectiveMatrix(&camera, view_handle);
	matrix4F mv = proj * view;

	auto entity_model = entity_registry.view<Model, WorldTransform>();
	for (auto entity : entity_model)
	{
		auto &model = entity_model.get<Model>(entity);
		auto &transform = entity_model.get<WorldTransform>(entity);

		Mesh* mesh = &this->loaded_meshes[model.mesh];
		TextureHandle texture = this->loaded_textures[model.texture];

		matrix4F translation = glm::translate(matrix4F(1.0F), (vector3F)(transform.current_transform.getPosition()));
		matrix4F orientation = glm::toMat4((quaternionF)transform.current_transform.getOrientation());
		matrix4F mvp = mv * (translation * orientation);
		this->backend->drawMeshView(view_handle, 0, mesh->vertices, mesh->indices, mesh->indices_count, texture, mvp);
	}

	this->backend->endView(view_handle);
	this->backend->sumbitView(view_handle);
}
