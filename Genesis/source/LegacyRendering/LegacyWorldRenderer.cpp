#include "Genesis/LegacyRendering/LegacyWorldRenderer.hpp"

#include "Genesis/Platform/FileSystem.hpp"
#include "Genesis/Rendering/Camera.hpp"

#include "Genesis/Resource/PbrMesh.hpp"
#include "Genesis/Resource/PbrMaterial.hpp"

#include "Genesis/Rendering/Lights.hpp"

namespace Genesis
{
	struct LegacyShaderUniform
	{
		static void writeEnvironment(LegacyBackend* backend, vector3F ambient_light, vector3F camera_position, matrix4F view_projection_matrix)
		{
			backend->setUniform3f("environment.ambient_light", ambient_light);
			backend->setUniform3f("environment.camera_position", camera_position);
			backend->setUniformMat4f("environment.view_projection_matrix", view_projection_matrix);
		}

		static void writeMaterialUniform(LegacyBackend* backend, const PbrMaterial& material)
		{
			backend->setUniform4f("material.albedo", material.albedo_factor);
			backend->setUniform2f("material.metallic_roughness", material.metallic_roughness_factor);
			backend->setUniform4f("material.emissive", material.emissive_factor);

			backend->setUniform1i("material.albedo_uv", material.albedo_uv);
			backend->setUniform1i("material.metallic_roughness_uv", material.metallic_roughness_uv);
			backend->setUniform1i("material.normal_uv", material.normal_uv);
			backend->setUniform1i("material.occlusion_uv", material.occlusion_uv);
			backend->setUniform1i("material.emissive_uv", material.emissive_uv);

			if (material.albedo_texture != nullptr)
			{
				backend->setUniformTexture("material.albedo_texture", 0, material.albedo_texture);
			}

			if (material.metallic_roughness_texture != nullptr)
			{
				backend->setUniformTexture("material.metallic_roughness_texture", 1, material.metallic_roughness_texture);
			}

			if (material.normal_texture != nullptr)
			{
				backend->setUniformTexture("material.normal_texture", 2, material.normal_texture);
			}

			if (material.occlusion_texture != nullptr)
			{
				backend->setUniformTexture("material.occlusion_texture", 3, material.occlusion_texture);
			}

			if (material.emissive_texture != nullptr)
			{
				backend->setUniformTexture("material.emissive_texture", 4, material.emissive_texture);
			}
		}

		static void writeTransformUniform(LegacyBackend* backend, TransformD& transform)
		{
			backend->setUniformMat4f("matrices.model", transform.getModelMatrix());
			backend->setUniformMat3f("matrices.normal", transform.getNormalMatrix());
		}

		static void writeDirectionalLight(LegacyBackend* backend, const DirectionalLight& light, const vector3F& light_direction)
		{
			backend->setUniform3f("directional_light.base.color", light.color);
			backend->setUniform1f("directional_light.base.intensity", light.intensity);
			backend->setUniform3f("directional_light.direction", light_direction);
		}
	};


	LegacyWorldRenderer::LegacyWorldRenderer(LegacyBackend* backend)
	{
		this->backend = backend;

		//glTF.vert
		string vert_data = "";
		string frag_data = "";

		FileSystem::loadFileString("res/shaders_opengl/Model.vert", vert_data);
		FileSystem::loadFileString("res/shaders_opengl/Model.frag", frag_data);
		this->ambient_program = this->backend->createShaderProgram(vert_data.data(), (uint32_t)vert_data.size(), frag_data.data(), (uint32_t)frag_data.size());

		//ModelDirectional.frag
		frag_data.clear();
		FileSystem::loadFileString("res/shaders_opengl//ModelDirectional.frag", frag_data);
		this->directional_program = this->backend->createShaderProgram(vert_data.data(), (uint32_t)vert_data.size(), frag_data.data(), (uint32_t)frag_data.size());
	}

	LegacyWorldRenderer::~LegacyWorldRenderer()
	{
		this->backend->destoryShaderProgram(this->ambient_program);
	}

	void LegacyWorldRenderer::drawScene(vector2U target_size, Framebuffer target_framebuffer, EntityWorld* world, Camera& camera, TransformD& camera_transform)
	{
		this->backend->bindFramebuffer(target_framebuffer);
		this->backend->clearFramebuffer(true, true);

		matrix4F view_projection_matrix = camera.getProjectionMatrix(target_size) * camera_transform.getViewMatirx();

		/*const ComponentId TransformId = world->getComponentID<TransformD>();
		const ComponentId MeshId = world->getComponentID<PbrMesh>();
		const ComponentId MaterialId = world->getComponentID<PbrMaterial>();
		const ComponentId DirectionalId = world->getComponentID<DirectionalLight>();*/

		//auto entity_mesh_pools = world->getEntityPools<TransformD, PbrMesh, PbrMaterial>();

		const PipelineSettings ambient_settings = { CullMode::Back, DepthTest::Test_And_Write, DepthOp::Less, BlendOp::None,  BlendFactor::One, BlendFactor::Zero };
		this->backend->setPipelineState(ambient_settings);

		//Draw ambient pass
		{
			this->backend->bindShaderProgram(this->ambient_program);

			LegacyShaderUniform::writeEnvironment(this->backend, vector3F(0.1f), (vector3F)camera_transform.getPosition(), view_projection_matrix);

			/*for (EntityPool* pool : *entity_mesh_pools)
			{
				for (size_t i = 0; i < pool->getEntityCount(); i++)
				{
					TransformD* transform = (TransformD*)pool->getComponentIndex(i, TransformId);
					PbrMesh* mesh = (PbrMesh*)pool->getComponentIndex(i, MeshId);
					PbrMaterial* material = (PbrMaterial*)pool->getComponentIndex(i, MaterialId);

					LegacyShaderUniform::writeTransformUniform(this->backend, *transform);
					LegacyShaderUniform::writeMaterialUniform(this->backend, *material);

					this->backend->bindVertexBuffer(mesh->vertex_buffer);
					this->backend->bindIndexBuffer(mesh->index_buffer);

					this->backend->drawIndex(mesh->index_count, 0);
				}
			}*/
		}

		const PipelineSettings light_settings = { CullMode::Back, DepthTest::Test_Only, DepthOp::Equal, BlendOp::Add,  BlendFactor::One, BlendFactor::One };
		this->backend->setPipelineState(light_settings);

		//Draw directional light pass
		{
			/*auto directional_light_pools = world->getEntityPools<TransformD, DirectionalLight>();

			this->backend->bindShaderProgram(this->directional_program);

			LegacyShaderUniform::writeEnvironment(this->backend, vector3F(0.1f), (vector3F)camera_transform.getPosition(), view_projection_matrix);

			for (EntityPool* pool : *entity_mesh_pools)
			{
				for (size_t i = 0; i < pool->getEntityCount(); i++)
				{
					TransformD* transform = (TransformD*)pool->getComponentIndex(i, TransformId);
					PbrMesh* mesh = (PbrMesh*)pool->getComponentIndex(i, MeshId);
					PbrMaterial* material = (PbrMaterial*)pool->getComponentIndex(i, MaterialId);

					LegacyShaderUniform::writeTransformUniform(this->backend, *transform);
					LegacyShaderUniform::writeMaterialUniform(this->backend, *material);

					this->backend->bindVertexBuffer(mesh->vertex_buffer);
					this->backend->bindIndexBuffer(mesh->index_buffer);

					//For each light
					for (EntityPool* light_pool : *directional_light_pools)
					{
						for (size_t i = 0; i < light_pool->getEntityCount(); i++)
						{
							TransformD* light_transform = (TransformD*)light_pool->getComponentIndex(i, TransformId);
							DirectionalLight* light = (DirectionalLight*)light_pool->getComponentIndex(i, DirectionalId);
							
							if (light->enabled)
							{
								LegacyShaderUniform::writeDirectionalLight(this->backend, *light, (vector3F)light_transform->getForward());
								this->backend->drawIndex(mesh->index_count, 0);
							}
						}
					}

				}
			}*/
		}

		this->backend->bindShaderProgram(nullptr);
		this->backend->bindFramebuffer(nullptr);
	}
}