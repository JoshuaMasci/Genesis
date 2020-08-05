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

		string comp_data = "";
		FileSystem::loadFileString("res/shaders_opengl/GammaCorrection.glsl", comp_data);
		this->gamma_correction_program = this->backend->createComputeShader(comp_data.data(), (uint32_t)comp_data.size());
	}

	LegacyWorldRenderer::~LegacyWorldRenderer()
	{
		this->backend->destoryShaderProgram(this->ambient_program);
		this->backend->destoryShaderProgram(this->directional_program);
		this->backend->destoryShaderProgram(this->gamma_correction_program);
	}

	void LegacyWorldRenderer::drawScene(vector2U target_size, Framebuffer target_framebuffer, EntityRegisty& world, Camera& camera, TransformD& camera_transform)
	{
		this->backend->bindFramebuffer(target_framebuffer);
		this->backend->clearFramebuffer(true, true);

		matrix4F view_projection_matrix = camera.getProjectionMatrix(target_size) * camera_transform.getViewMatirx();

		const PipelineSettings ambient_settings = { CullMode::Back, DepthTest::Test_And_Write, DepthOp::Less, BlendOp::None, BlendFactor::One, BlendFactor::Zero };
		this->backend->setPipelineState(ambient_settings);

		struct MeshStruct
		{
			PbrMesh mesh;
			PbrMaterial material;
			TransformD transform;
		};
		static vector<MeshStruct> meshes;
		meshes.clear();
		{
			auto mesh_group = world.view<PbrMesh, PbrMaterial, TransformD>();
			for (EntityHandle entity : mesh_group)
			{
				auto&[mesh, material, transform] = mesh_group.get<PbrMesh, PbrMaterial, TransformD>(entity);
				meshes.push_back({ mesh , material, transform });
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

		//Draw ambient pass
		{
			this->backend->bindShaderProgram(this->ambient_program);

			LegacyShaderUniform::writeEnvironment(this->backend, vector3F(0.1f), (vector3F)camera_transform.getPosition(), view_projection_matrix);

			for (MeshStruct& mesh : meshes)
			{
				LegacyShaderUniform::writeTransformUniform(this->backend, mesh.transform);
				LegacyShaderUniform::writeMaterialUniform(this->backend, mesh.material);

				this->backend->bindVertexBuffer(mesh.mesh.vertex_buffer);
				this->backend->bindIndexBuffer(mesh.mesh.index_buffer);

				this->backend->drawIndex(mesh.mesh.index_count, 0);
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
				LegacyShaderUniform::writeMaterialUniform(this->backend, mesh.material);

				this->backend->bindVertexBuffer(mesh.mesh.vertex_buffer);
				this->backend->bindIndexBuffer(mesh.mesh.index_buffer);

				for (DirectionalLightStruct& light : directional_lights)
				{
					if (light.light.enabled)
					{
						LegacyShaderUniform::writeDirectionalLight(this->backend, light.light, (vector3F)light.transform.getForward());
						this->backend->drawIndex(mesh.mesh.index_count, 0);
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