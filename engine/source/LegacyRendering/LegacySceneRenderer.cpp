#include "genesis_engine/LegacyRendering/LegacySceneRenderer.hpp"

#include "genesis_engine/platform/file_system.hpp"

namespace genesis
{
	struct LegacyShaderUniform
	{
		static void write_environment(LegacyBackend* backend, vec3f ambient_light, vec3f camera_position, mat4f view_projection_matrix)
		{
			backend->setUniform3f("environment.ambient_light", ambient_light);
			backend->setUniform3f("environment.camera_position", camera_position);
			backend->setUniformMat4f("environment.view_projection_matrix", view_projection_matrix);
		}

		static void write_material_uniform(LegacyBackend* backend, const Material& material)
		{
			backend->setUniform4f("material.albedo", material.albedo_factor);
			backend->setUniform2f("material.metallic_roughness", material.metallic_roughness_factor);
			backend->setUniform4f("material.emissive", material.emissive_factor);
			
			backend->setUniform1i("material.albedo_uv", material.albedo_texture.uv);
			if (material.albedo_texture.uv != -1 && material.albedo_texture.texture)
			{
				backend->setUniformTexture("material.albedo_texture", 0, material.albedo_texture.texture->texture);
			}

			backend->setUniform1i("material.metallic_roughness_uv", material.metallic_roughness_texture.uv);
			if (material.metallic_roughness_texture.uv != -1 && material.metallic_roughness_texture.texture)
			{
				backend->setUniformTexture("material.metallic_roughness_texture", 1, material.metallic_roughness_texture.texture->texture);
			}

			backend->setUniform1i("material.normal_uv", material.normal_texture.uv);
			if (material.normal_texture.uv != -1 && material.normal_texture.texture)
			{
				backend->setUniformTexture("material.normal_texture", 2, material.normal_texture.texture->texture);
			}

			backend->setUniform1i("material.occlusion_uv", material.occlusion_texture.uv);
			if (material.occlusion_texture.uv != -1 && material.occlusion_texture.texture)
			{
				backend->setUniformTexture("material.occlusion_texture", 3, material.occlusion_texture.texture->texture);
			}

			backend->setUniform1i("material.emissive_uv", material.emissive_texture.uv);
			if (material.emissive_texture.uv != -1 && material.emissive_texture.texture)
			{
				backend->setUniformTexture("material.emissive_texture", 4, material.emissive_texture.texture->texture);
			}
		}

		static void write_transform_uniform(LegacyBackend* backend, TransformD& transform)
		{
			backend->setUniformMat4f("matrices.model", transform.getModelMatrix());
			backend->setUniformMat3f("matrices.normal", transform.getNormalMatrix());
		}

		static void write_directional_light(LegacyBackend* backend, const DirectionalLight& light, const vec3f& light_direction)
		{
			backend->setUniform3f("directional_light.base.color", light.color);
			backend->setUniform1f("directional_light.base.intensity", light.intensity);
			backend->setUniform3f("directional_light.direction", light_direction);
		}

		static void write_point_light(LegacyBackend* backend, const PointLight& light, const vec3f& light_position)
		{
			backend->setUniform3f("point_light.base.color", light.color);
			backend->setUniform1f("point_light.base.intensity", light.intensity);
			backend->setUniform1f("point_light.range", light.range);
			backend->setUniform2f("point_light.attenuation", light.attenuation);
			backend->setUniform3f("point_light.position", light_position);
		}
	};

	LegacySceneRenderer::LegacySceneRenderer(LegacyBackend* backend)
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

	LegacySceneRenderer::~LegacySceneRenderer()
	{
		this->backend->destoryShaderProgram(this->ambient_program);
		this->backend->destoryShaderProgram(this->directional_program);
		this->backend->destoryShaderProgram(this->point_program);
		this->backend->destoryShaderProgram(this->gamma_correction_program);
	}

	void LegacySceneRenderer::draw_scene(vec2u target_size, Framebuffer target_framebuffer, SceneRenderList& render_list, SceneLightingSettings& lighting, RenderSettings& settings, CameraStruct& active_camera)
	{
		this->backend->bindFramebuffer(target_framebuffer);
		this->backend->clearFramebuffer(true, true);

		mat4f view_projection_matrix = active_camera.camera.get_projection_matrix(target_size) * active_camera.transform.getViewMatirx();

		const PipelineSettings ambient_settings = { CullMode::Back, DepthTest::Test_And_Write, DepthOp::Less, BlendOp::None, BlendFactor::One, BlendFactor::Zero };
		this->backend->setPipelineState(ambient_settings);

		//Draw ambient pass
		{
			this->backend->bindShaderProgram(this->ambient_program);

			LegacyShaderUniform::write_environment(this->backend, lighting.ambient_light, (vec3f)active_camera.transform.getPosition(), view_projection_matrix);

			for (ModelStruct& mesh : render_list.models)
			{
				LegacyShaderUniform::write_transform_uniform(this->backend, mesh.transform);
				LegacyShaderUniform::write_material_uniform(this->backend, *mesh.material);

				this->backend->bindVertexBuffer(mesh.mesh->vertex_buffer);
				this->backend->bindIndexBuffer(mesh.mesh->index_buffer);

				this->backend->drawIndex(mesh.mesh->index_count, 0);
			}
		}

		const PipelineSettings light_settings = { CullMode::Back, DepthTest::Test_Only, DepthOp::Equal, BlendOp::Add, BlendFactor::One, BlendFactor::One };
		this->backend->setPipelineState(light_settings);

		if (settings.lighting)
		{
			//Draw directional light pass
			{
				this->backend->bindShaderProgram(this->directional_program);
				LegacyShaderUniform::write_environment(this->backend, lighting.ambient_light, (vec3f)active_camera.transform.getPosition(), view_projection_matrix);

				for (ModelStruct& mesh : render_list.models)
				{
					LegacyShaderUniform::write_transform_uniform(this->backend, mesh.transform);
					LegacyShaderUniform::write_material_uniform(this->backend, *mesh.material);

					this->backend->bindVertexBuffer(mesh.mesh->vertex_buffer);
					this->backend->bindIndexBuffer(mesh.mesh->index_buffer);

					for (DirectionalLightStruct& light : render_list.directional_lights)
					{
						if (light.light.enabled)
						{
							LegacyShaderUniform::write_directional_light(this->backend, light.light, (vec3f)light.transform.getForward());
							this->backend->drawIndex(mesh.mesh->index_count, 0);
						}
					}
				}
			}

			//Draw point light pass
			{
				this->backend->bindShaderProgram(this->point_program);
				LegacyShaderUniform::write_environment(this->backend, lighting.ambient_light, (vec3f)active_camera.transform.getPosition(), view_projection_matrix);

				for (ModelStruct& mesh : render_list.models)
				{
					LegacyShaderUniform::write_transform_uniform(this->backend, mesh.transform);
					LegacyShaderUniform::write_material_uniform(this->backend, *mesh.material);

					this->backend->bindVertexBuffer(mesh.mesh->vertex_buffer);
					this->backend->bindIndexBuffer(mesh.mesh->index_buffer);

					for (PointLightStruct& light : render_list.point_lights)
					{
						if (light.light.enabled)
						{
							LegacyShaderUniform::write_point_light(this->backend, light.light, (vec3f)light.transform.getPosition());
							this->backend->drawIndex(mesh.mesh->index_count, 0);
						}
					}
				}
			}
		}

		this->backend->bindFramebuffer(nullptr);

		//Gamma Correction
		this->backend->bindShaderProgram(this->gamma_correction_program);
		this->backend->setUniform1f("gamma", lighting.gamma_correction);
		this->backend->setUniformTextureImage("target", 0, this->backend->getFramebufferColorAttachment(target_framebuffer, 0));
		this->backend->dispatchCompute(target_size.x, target_size.y, 1);
		this->backend->bindShaderProgram(nullptr);
	}
}