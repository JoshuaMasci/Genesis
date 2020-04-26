#include "Genesis/LegacyRendering/LegacyWorldRenderer.hpp"

#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Frustum.hpp"

#include "Genesis/Platform/FileSystem.hpp"

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

		FileSystem::loadFileString("res/shaders_opengl/Model.vert", vert_data);

		FileSystem::loadFileString("res/shaders_opengl/build/model_albedo_basic_basic_ambient.frag", frag_data);
		this->color_ambient = this->legacy_backend->createShaderProgram(vert_data.data(), (uint32_t)vert_data.size(), frag_data.data(), (uint32_t)frag_data.size());

		FileSystem::loadFileString("res/shaders_opengl/build/model_texture_basic_basic_ambient.frag", frag_data);
		this->texture_ambient = this->legacy_backend->createShaderProgram(vert_data.data(), (uint32_t)vert_data.size(), frag_data.data(), (uint32_t)frag_data.size());

		FileSystem::loadFileString("res/shaders_opengl/build/model_albedo_basic_basic_directional.frag", frag_data);
		this->color_directional = this->legacy_backend->createShaderProgram(vert_data.data(), (uint32_t)vert_data.size(), frag_data.data(), (uint32_t)frag_data.size());
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

	auto& view = world->getEntityRegistry()->view<LegacyMeshComponent, TransformD>();
	for (EntityHandle entity : view)
	{
		LegacyMeshComponent& mesh_component = view.get<LegacyMeshComponent>(entity);
		TransformF render_transform = view.get<TransformD>(entity).toTransformF();

		if (frustum.sphereTest(render_transform.getPosition(), mesh_component.mesh->frustum_sphere_radius))
		{
			if (mesh_component.material->texture_names[0].empty())
			{
				this->legacy_backend->bindShaderProgram(this->color_ambient);
			}
			else
			{
				this->legacy_backend->bindShaderProgram(this->texture_ambient);
			}

			//Environment
			{
				this->legacy_backend->setUniform3f("environment.camera_position", camera_transform.getPosition());
				this->legacy_backend->setUniform3f("environment.ambient_light", vector3F(0.4f));
				this->legacy_backend->setUniformMat4f("environment.view_projection_matrix", view_projection_matrix);
			}

			//Material
			{
				this->legacy_backend->setUniform4f("material.albedo", mesh_component.material->albedo);
				this->legacy_backend->setUniform4f("material.pbr_values", mesh_component.material->pbr_values);

				for (size_t i = 0; i < LegacyMaterial::TextureCount; i++)
				{
					if (!mesh_component.material->texture_names[i].empty())
					{
						this->legacy_backend->setUniformTexture("material_textures[" + std::to_string(i) + "]", (uint32_t)i, mesh_component.material->textures[i]);
					}
				}
			}

			//Matrices
			{
				this->legacy_backend->setUniformMat4f("matrices.model", render_transform.getModelMatrix());
				this->legacy_backend->setUniformMat3f("matrices.normal", render_transform.getNormalMatrix());
			}
		
			this->legacy_backend->draw(mesh_component.mesh->vertex_buffer, mesh_component.mesh->index_buffer, mesh_component.mesh->index_count);


			//TestLight
			if (mesh_component.material->texture_names[0].empty())
			{
				this->legacy_backend->TEMP_enableAlphaBlending(true);

				this->legacy_backend->bindShaderProgram(this->color_directional);

				//Environment
				{
					this->legacy_backend->setUniform3f("environment.camera_position", camera_transform.getPosition());
					this->legacy_backend->setUniform3f("environment.ambient_light", vector3F(0.4f));
					this->legacy_backend->setUniformMat4f("environment.view_projection_matrix", view_projection_matrix);
				}

				//Material
				{
					this->legacy_backend->setUniform4f("material.albedo", mesh_component.material->albedo);
					this->legacy_backend->setUniform4f("material.pbr_values", mesh_component.material->pbr_values);

					for (size_t i = 0; i < LegacyMaterial::TextureCount; i++)
					{
						if (!mesh_component.material->texture_names[i].empty())
						{
							this->legacy_backend->setUniformTexture("material_textures[" + std::to_string(i) + "]", (uint32_t)i, mesh_component.material->textures[i]);
						}
					}
				}

				//Matrices
				{
					this->legacy_backend->setUniformMat4f("matrices.model", render_transform.getModelMatrix());
					this->legacy_backend->setUniformMat3f("matrices.normal", render_transform.getNormalMatrix());
				}

				//Light
				this->legacy_backend->setUniform3f("directional_light.base.color", vector3F(0.75f, 0.0f, 0.6f));
				this->legacy_backend->setUniform1f("directional_light.base.intensity", 0.7f);
				this->legacy_backend->setUniform3f("directional_light.direction", glm::normalize(vector3F(0.1f, -1.0f, 0.1f)));

				this->legacy_backend->draw(mesh_component.mesh->vertex_buffer, mesh_component.mesh->index_buffer, mesh_component.mesh->index_count);

				this->legacy_backend->TEMP_enableAlphaBlending(false);
			}
		}
	}
}
