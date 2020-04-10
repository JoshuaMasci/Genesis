#include "Genesis/LegacyRendering/LegacyWorldRenderer.hpp"

#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Frustum.hpp"

#include <fstream>

using namespace Genesis;

LegacyWorldRenderer::LegacyWorldRenderer(LegacyBackend* backend)
{
	this->legacy_backend = backend;

	this->mesh_pool = new LegacyMeshPool(this->legacy_backend);
	this->texture_pool = new LegacyTexturePool(this->legacy_backend);
	this->material_pool = new LegacyMaterialPool(this->legacy_backend, this->texture_pool);

	{
		string vert_data = "";
		string frag_data = "";

		std::ifstream vert_file("res/shaders_opengl/Model.vert", std::ios::ate | std::ios::binary);
		if (vert_file.is_open())
		{
			size_t fileSize = (size_t)vert_file.tellg();
			vert_file.seekg(0);
			vert_data.resize(fileSize);
			vert_data.assign((std::istreambuf_iterator<char>(vert_file)), (std::istreambuf_iterator<char>()));
			vert_file.close();
		}

		std::ifstream frag_file("res/shaders_opengl/Model.frag", std::ios::ate | std::ios::binary);
		if (frag_file.is_open())
		{
			size_t fileSize = (size_t)frag_file.tellg();
			frag_file.seekg(0);
			frag_data.resize(fileSize);
			frag_data.assign((std::istreambuf_iterator<char>(frag_file)), (std::istreambuf_iterator<char>()));
			frag_file.close();
		}

		this->mesh_program = this->legacy_backend->createShaderProgram(vert_data.data(), (uint32_t)vert_data.size(), frag_data.data(), (uint32_t)frag_data.size());
	}
}

LegacyWorldRenderer::~LegacyWorldRenderer()
{
	delete this->material_pool;
	delete this->texture_pool;
	delete this->mesh_pool;
}

void LegacyWorldRenderer::addEntity(EntityRegistry* registry, EntityHandle entity, const string& mesh_file, const string& material_file)
{
	registry->assign<LegacyMeshComponent>(entity, this->mesh_pool->getResource(mesh_file), this->material_pool->getResource(material_file));
}

void LegacyWorldRenderer::removeEntity(EntityRegistry* registry, EntityHandle entity)
{
	//TODO unload
	registry->remove<LegacyMeshComponent>(entity);
}

void LegacyWorldRenderer::drawWorld(World* world)
{
	vector2U temp_size = this->legacy_backend ->getScreenSize();
	float aspect_ratio = ((float)temp_size.x) / ((float)temp_size.y);

	EntityHandle camera = world->getCamera();
	Camera& camera_component = world->getEntityRegistry()->get<Camera>(camera);
	TransformF camera_transform = world->getEntityRegistry()->get<TransformD>(camera).toTransformF();
	matrix4F view_projection_matrix = camera_component.getProjectionMatrix(aspect_ratio) * camera_transform.getViewMatirx();

	Frustum frustum(view_projection_matrix);

	this->legacy_backend->bindShaderProgram(this->mesh_program);

	//Environment
	{
		this->legacy_backend->setUniform3f("environment.camera_position", camera_transform.getPosition());
		this->legacy_backend->setUniform3f("environment.ambient_light", vector3F(1.0f));
		this->legacy_backend->setUniformMat4f("environment.view_projection_matrix", view_projection_matrix);
	}

	auto& view = world->getEntityRegistry()->view<LegacyMeshComponent, TransformD>();
	for (EntityHandle entity : view)
	{
		LegacyMeshComponent& mesh_component = view.get<LegacyMeshComponent>(entity);
		TransformF render_transform = view.get<TransformD>(entity).toTransformF();

		if (frustum.sphereTest(render_transform.getPosition(), mesh_component.mesh->frustum_sphere_radius))
		{
			//Material
			{
				this->legacy_backend->setUniform4f("material.albedo", mesh_component.material->albedo);
				this->legacy_backend->setUniform4f("material.pbr_values", mesh_component.material->pbr_values);

				vector4U has_textures1(0);
				for (size_t i = 0; i < 4; i++)
				{
					if (!mesh_component.material->texture_names[i].empty())
					{
						has_textures1[i] = 1;
					}
				}
				this->legacy_backend->setUniform4u("material.has_textures1", has_textures1);

				vector4U has_textures2(0);
				for (size_t i = 0; i < 4; i++)
				{
					if (!mesh_component.material->texture_names[i + 4].empty())
					{
						has_textures1[i] = 1;
					}
				}
				this->legacy_backend->setUniform4u("material.has_textures2", has_textures2);

				for (size_t i = 0; i < LegacyMaterial::TextureCount; i++)
				{
					if (!mesh_component.material->texture_names[i].empty())
					{
						this->legacy_backend->setUniformTexture("material_textures[" + std::to_string(i) + "]", i, mesh_component.material->textures[i]);
					}
				}



			}

			//Matrices
			{
				this->legacy_backend->setUniformMat4f("matrices.model", render_transform.getModelMatrix());
				this->legacy_backend->setUniformMat3f("matrices.normal", render_transform.getNormalMatrix());
			}
		
			this->legacy_backend->draw(mesh_component.mesh->vertex_buffer, mesh_component.mesh->index_buffer, mesh_component.mesh->index_count);
		}
	}
}
