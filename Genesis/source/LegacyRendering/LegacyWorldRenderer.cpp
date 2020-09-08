#include "Genesis/LegacyRendering/LegacyWorldRenderer.hpp"

#include "Genesis/Platform/FileSystem.hpp"

#include "Genesis/Component/MeshComponent.hpp"

#include "Genesis/Rendering/Camera.hpp"
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

		static void writeMaterialUniform(LegacyBackend* backend, const Material& material)
		{
			backend->setUniform4f("material.albedo", material.albedo_factor);
			backend->setUniform2f("material.metallic_roughness", material.metallic_roughness_factor);
			backend->setUniform4f("material.emissive", material.emissive_factor);
			
			backend->setUniform1i("material.albedo_uv", material.albedo_texture.uv);
			if (material.albedo_texture.uv != -1)
			{
				backend->setUniformTexture("material.albedo_texture", 0, material.albedo_texture.texture);
			}

			backend->setUniform1i("material.metallic_roughness_uv", material.metallic_roughness_texture.uv);
			if (material.metallic_roughness_texture.uv != -1)
			{
				backend->setUniformTexture("material.metallic_roughness_texture", 1, material.metallic_roughness_texture.texture);
			}

			backend->setUniform1i("material.normal_uv", material.normal_texture.uv);
			if (material.normal_texture.uv != -1)
			{
				backend->setUniformTexture("material.normal_texture", 2, material.normal_texture.texture);
			}

			backend->setUniform1i("material.occlusion_uv", material.occlusion_texture.uv);
			if (material.occlusion_texture.uv != -1)
			{
				backend->setUniformTexture("material.occlusion_texture", 3, material.occlusion_texture.texture);
			}

			backend->setUniform1i("material.emissive_uv", material.emissive_texture.uv);
			if (material.emissive_texture.uv != -1)
			{
				backend->setUniformTexture("material.emissive_texture", 4, material.emissive_texture.texture);
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

		static void writePointLight(LegacyBackend* backend, const PointLight& light, const vector3F& light_position)
		{
			backend->setUniform3f("point_light.base.color", light.color);
			backend->setUniform1f("point_light.base.intensity", light.intensity);
			backend->setUniform1f("point_light.range", light.range);
			backend->setUniform2f("point_light.attenuation", light.attenuation);
			backend->setUniform3f("point_light.position", light_position);
		}
	};

	LegacyWorldRenderer::LegacyWorldRenderer(LegacyBackend* backend)
	{
		this->backend = backend;

		string vert_data = "";
		string frag_data = "";

		FileSystem::loadShaderString("res/shaders_opengl/Model.vert", vert_data);
		FileSystem::loadShaderString("res/shaders_opengl/Model.frag", frag_data);
		this->ambient_program = this->backend->createShaderProgram(vert_data.data(), (uint32_t)vert_data.size(), frag_data.data(), (uint32_t)frag_data.size());

		frag_data.clear();
		FileSystem::loadShaderString("res/shaders_opengl/ModelDirectional.frag", frag_data);
		this->directional_program = this->backend->createShaderProgram(vert_data.data(), (uint32_t)vert_data.size(), frag_data.data(), (uint32_t)frag_data.size());

		frag_data.clear();
		FileSystem::loadShaderString("res/shaders_opengl/ModelPoint.frag", frag_data);
		this->point_program = this->backend->createShaderProgram(vert_data.data(), (uint32_t)vert_data.size(), frag_data.data(), (uint32_t)frag_data.size());

		string comp_data = "";
		FileSystem::loadShaderString("res/shaders_opengl/GammaCorrection.glsl", comp_data);
		this->gamma_correction_program = this->backend->createComputeShader(comp_data.data(), (uint32_t)comp_data.size());
	}

	LegacyWorldRenderer::~LegacyWorldRenderer()
	{
		this->backend->destoryShaderProgram(this->ambient_program);
		this->backend->destoryShaderProgram(this->directional_program);
		this->backend->destoryShaderProgram(this->point_program);
		this->backend->destoryShaderProgram(this->gamma_correction_program);
	}

	void LegacyWorldRenderer::drawScene(vector2U target_size, Framebuffer target_framebuffer, EntityRegistry& world, Camera& camera, TransformD& camera_transform)
	{
		this->backend->bindFramebuffer(target_framebuffer);
		this->backend->clearFramebuffer(true, true);

		matrix4F view_projection_matrix = camera.getProjectionMatrix(target_size) * camera_transform.getViewMatirx();

		const PipelineSettings ambient_settings = { CullMode::Back, DepthTest::Test_And_Write, DepthOp::Less, BlendOp::None, BlendFactor::One, BlendFactor::Zero };
		this->backend->setPipelineState(ambient_settings);

		struct MeshStruct
		{
			Mesh* mesh;
			Material* material;
			TransformD transform;
		};
		static vector<MeshStruct> meshes;
		meshes.clear();
		{
			auto mesh_group = world.view<MeshComponent, TransformD>();
			for (EntityHandle entity : mesh_group)
			{
				MeshComponent& mesh_component = mesh_group.get<MeshComponent>(entity);
				TransformD& transform = mesh_group.get<TransformD>(entity);

				if (mesh_component.mesh != nullptr && mesh_component.material != nullptr)
				{
					meshes.push_back({ mesh_component.mesh , mesh_component.material, transform });
				}
			}
		}

		struct DirectionalLightStruct
		{
			DirectionalLight light;
			TransformD transform;
		};
		static vector<DirectionalLightStruct> directional_lights;
		directional_lights.clear();
		{
			auto directional_light_group = world.view<DirectionalLight, TransformD>();
			for (EntityHandle entity : directional_light_group)
			{
				auto&[light, transform] = directional_light_group.get<DirectionalLight, TransformD>(entity);
				directional_lights.push_back({light, transform});
			}
		}

		struct PointLightStruct
		{
			PointLight light;
			TransformD transform;
		};
		static vector<PointLightStruct> point_lights;
		point_lights.clear();
		{
			auto point_light_group = world.view<PointLight, TransformD>();
			for (EntityHandle entity : point_light_group)
			{
				auto&[light, transform] = point_light_group.get<PointLight, TransformD>(entity);
				point_lights.push_back({ light, transform });
			}
		}

		//Draw ambient pass
		{
			this->backend->bindShaderProgram(this->ambient_program);

			LegacyShaderUniform::writeEnvironment(this->backend, vector3F(0.1f), (vector3F)camera_transform.getPosition(), view_projection_matrix);

			for (MeshStruct& mesh : meshes)
			{
				LegacyShaderUniform::writeTransformUniform(this->backend, mesh.transform);
				LegacyShaderUniform::writeMaterialUniform(this->backend, *mesh.material);

				this->backend->bindVertexBuffer(mesh.mesh->vertex_buffer);
				this->backend->bindIndexBuffer(mesh.mesh->index_buffer);

				this->backend->drawIndex(mesh.mesh->index_count, 0);
			}
		}

		const PipelineSettings light_settings = { CullMode::Back, DepthTest::Test_Only, DepthOp::Equal, BlendOp::Add, BlendFactor::One, BlendFactor::One };
		this->backend->setPipelineState(light_settings);

		//Draw directional light pass
		{
			this->backend->bindShaderProgram(this->directional_program);
			LegacyShaderUniform::writeEnvironment(this->backend, vector3F(0.1f), (vector3F)camera_transform.getPosition(), view_projection_matrix);

			for (MeshStruct& mesh : meshes)
			{
				LegacyShaderUniform::writeTransformUniform(this->backend, mesh.transform);
				LegacyShaderUniform::writeMaterialUniform(this->backend, *mesh.material);

				this->backend->bindVertexBuffer(mesh.mesh->vertex_buffer);
				this->backend->bindIndexBuffer(mesh.mesh->index_buffer);

				for (DirectionalLightStruct& light : directional_lights)
				{
					if (light.light.enabled)
					{
						LegacyShaderUniform::writeDirectionalLight(this->backend, light.light, (vector3F)light.transform.getForward());
						this->backend->drawIndex(mesh.mesh->index_count, 0);
					}
				}
			}
		}

		//Draw point light pass
		{
			this->backend->bindShaderProgram(this->point_program);
			LegacyShaderUniform::writeEnvironment(this->backend, vector3F(0.1f), (vector3F)camera_transform.getPosition(), view_projection_matrix);

			for (MeshStruct& mesh : meshes)
			{
				LegacyShaderUniform::writeTransformUniform(this->backend, mesh.transform);
				LegacyShaderUniform::writeMaterialUniform(this->backend, *mesh.material);

				this->backend->bindVertexBuffer(mesh.mesh->vertex_buffer);
				this->backend->bindIndexBuffer(mesh.mesh->index_buffer);

				for (PointLightStruct& light : point_lights)
				{
					if (light.light.enabled)
					{
						LegacyShaderUniform::writePointLight(this->backend, light.light, (vector3F)light.transform.getPosition());
						this->backend->drawIndex(mesh.mesh->index_count, 0);
					}
				}
			}
		}

		this->backend->bindFramebuffer(nullptr);

		//Gamma Correction
		this->backend->bindShaderProgram(this->gamma_correction_program);
		this->backend->setUniform1f("gamma", 2.2f);
		this->backend->setUniformTextureImage("target", 0, this->backend->getFramebufferColorAttachment(target_framebuffer, 0));
		this->backend->dispatchCompute(target_size.x, target_size.y, 1);
		this->backend->bindShaderProgram(nullptr);
	}
}