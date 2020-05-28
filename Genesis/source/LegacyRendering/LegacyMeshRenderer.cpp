#include "Genesis/LegacyRendering/LegacyMeshRenderer.hpp"

using namespace Genesis;

#include "Genesis/Platform/FileSystem.hpp"

LegacyMeshRenderer::LegacyMeshRenderer(LegacyBackend* backend)
{
	this->legacy_backend = backend;

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

		FileSystem::loadFileString("res/shaders_opengl/build/model_texture_basic_basic_directional.frag", frag_data);
		this->texture_directional = this->legacy_backend->createShaderProgram(vert_data.data(), (uint32_t)vert_data.size(), frag_data.data(), (uint32_t)frag_data.size());

		FileSystem::loadFileString("res/shaders_opengl/build/model_albedo_basic_basic_point.frag", frag_data);
		this->color_point = this->legacy_backend->createShaderProgram(vert_data.data(), (uint32_t)vert_data.size(), frag_data.data(), (uint32_t)frag_data.size());

		FileSystem::loadFileString("res/shaders_opengl/build/model_texture_basic_basic_point.frag", frag_data);
		this->texture_point = this->legacy_backend->createShaderProgram(vert_data.data(), (uint32_t)vert_data.size(), frag_data.data(), (uint32_t)frag_data.size());

		FileSystem::loadFileString("res/shaders_opengl/build/model_albedo_basic_basic_spot.frag", frag_data);
		this->color_spot = this->legacy_backend->createShaderProgram(vert_data.data(), (uint32_t)vert_data.size(), frag_data.data(), (uint32_t)frag_data.size());

		FileSystem::loadFileString("res/shaders_opengl/build/model_texture_basic_basic_spot.frag", frag_data);
		this->texture_spot = this->legacy_backend->createShaderProgram(vert_data.data(), (uint32_t)vert_data.size(), frag_data.data(), (uint32_t)frag_data.size());
	}
}

LegacyMeshRenderer::~LegacyMeshRenderer()
{

}

void writeEnvironmentUniform(LegacyBackend* backend, Genesis::SceneData* environment)
{
	backend->setUniform3f("environment.camera_position", environment->camera_position);
	backend->setUniform3f("environment.ambient_light", environment->ambient_light);
	backend->setUniformMat4f("environment.view_projection_matrix", environment->view_projection_matrix);
}

void writeMaterialUniform(LegacyBackend* backend, LegacyMaterial* material)
{
	backend->setUniform4f("material.albedo", material->albedo);
	backend->setUniform4f("material.pbr_values", material->pbr_values);

	for (size_t i = 0; i < LegacyMaterial::TextureCount; i++)
	{
		if (!material->texture_names[i].empty())
		{
			backend->setUniformTexture("material_textures[" + std::to_string(i) + "]", (uint32_t)i, material->textures[i]);
		}
	}
}

void writeTransformUniform(LegacyBackend* backend, TransformD& transform)
{
	backend->setUniformMat4f("matrices.model", transform.getModelMatrix());
	backend->setUniformMat3f("matrices.normal", transform.getNormalMatrix());
}

void writeDirectionalLightUniform(LegacyBackend* backend, const DirectionalLight& light, const TransformD& light_transform)
{
	backend->setUniform3f("directional_light.base.color", light.color);
	backend->setUniform1f("directional_light.base.intensity", light.intensity);
	backend->setUniform3f("directional_light.direction", light_transform.getForward());
}

void writePointLightUniform(LegacyBackend* backend, const PointLight& light, const TransformD& light_transform)
{
	backend->setUniform3f("point_light.base.color", light.color);
	backend->setUniform1f("point_light.base.intensity", light.intensity);
	backend->setUniform1f("point_light.range", light.range);
	backend->setUniform2f("point_light.attenuation", light.attenuation);
	backend->setUniform3f("point_light.position", light_transform.getPosition());
}

void writeSpotLightUniform(LegacyBackend* backend, const SpotLight& light, const TransformD& light_transform)
{
	backend->setUniform3f("spot_light.base.color", light.color);
	backend->setUniform1f("spot_light.base.intensity", light.intensity);
	backend->setUniform1f("spot_light.range", light.range);
	backend->setUniform2f("spot_light.attenuation", light.attenuation);
	backend->setUniform3f("spot_light.position", light_transform.getPosition());
	backend->setUniform3f("spot_light.direction", light_transform.getForward());
	backend->setUniform1f("spot_light.cutoff", light.cutoff);
}

#include "Genesis/Resource/PbrMesh.hpp"
#include "Genesis/Resource/PbrMaterial.hpp"

void LegacyMeshRenderer::drawAmbientPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum)
{
	auto& view = entity_registry->view<LegacyMeshComponent, LegacyMaterialComponent, TransformD>();
	for (EntityHandle entity : view)
	{
		LegacyMeshComponent& mesh_component = view.get<LegacyMeshComponent>(entity);
		LegacyMaterialComponent& material_component = view.get<LegacyMaterialComponent>(entity);
		if (mesh_component.mesh == nullptr && !mesh_component.mesh_file.empty())
		{
			mesh_component.mesh = this->mesh_pool->getResource(mesh_component.mesh_file);
		}
		if (material_component.material == nullptr && !material_component.material_file.empty())
		{
			material_component.material = this->material_pool->getResource(material_component.material_file);
		}

		TransformD& render_transform = view.get<TransformD>(entity);

		if (frustum->sphereTest((vector3F)render_transform.getPosition(), mesh_component.mesh->frustum_sphere_radius))
		{
			if (material_component.material->texture_names[0].empty())
			{
				this->legacy_backend->bindShaderProgram(this->color_ambient);
			}
			else
			{
				this->legacy_backend->bindShaderProgram(this->texture_ambient);
			}

			//Environment
			writeEnvironmentUniform(this->legacy_backend, environment);

			//Material
			writeMaterialUniform(this->legacy_backend, material_component.material);

			//Matrices
			writeTransformUniform(this->legacy_backend, render_transform);

			this->legacy_backend->draw(mesh_component.mesh->vertex_buffer, mesh_component.mesh->index_buffer, mesh_component.mesh->index_count);
		}
	}
}

#include "Genesis/Entity/Entity.hpp"
void LegacyMeshRenderer::drawEntity(Entity* entity)
{

}

void LegacyMeshRenderer::drawAmbientPass(World* world, SceneData* environment, Frustum* frustum)
{

}

void LegacyMeshRenderer::drawDirectionalPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, DirectionalLight& light, TransformD& light_transform)
{
	auto& view = entity_registry->view<LegacyMeshComponent, LegacyMaterialComponent, TransformD>();
	for (EntityHandle entity : view)
	{
		LegacyMeshComponent& mesh_component = view.get<LegacyMeshComponent>(entity);
		LegacyMaterialComponent& material_component = view.get<LegacyMaterialComponent>(entity);
		TransformD& render_transform = view.get<TransformD>(entity);

		if (frustum->sphereTest((vector3F)render_transform.getPosition(), mesh_component.mesh->frustum_sphere_radius))
		{
			if (material_component.material->texture_names[0].empty())
			{
				this->legacy_backend->bindShaderProgram(this->color_directional);
			}
			else
			{
				this->legacy_backend->bindShaderProgram(this->texture_directional);
			}

			//Environment
			writeEnvironmentUniform(this->legacy_backend, environment);

			//Material
			writeMaterialUniform(this->legacy_backend, material_component.material);

			//Matrices
			writeTransformUniform(this->legacy_backend, render_transform);

			//Light
			writeDirectionalLightUniform(this->legacy_backend, light, light_transform);

			this->legacy_backend->draw(mesh_component.mesh->vertex_buffer, mesh_component.mesh->index_buffer, mesh_component.mesh->index_count);
		}
	}
}

void LegacyMeshRenderer::drawPointPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, PointLight& light, TransformD& light_transform)
{
	auto& view = entity_registry->view<LegacyMeshComponent, LegacyMaterialComponent, TransformD>();
	for (EntityHandle entity : view)
	{
		LegacyMeshComponent& mesh_component = view.get<LegacyMeshComponent>(entity);
		LegacyMaterialComponent& material_component = view.get<LegacyMaterialComponent>(entity);
		TransformD& render_transform = view.get<TransformD>(entity);

		if (frustum->sphereTest((vector3F)render_transform.getPosition(), mesh_component.mesh->frustum_sphere_radius))
		{
			if (material_component.material->texture_names[0].empty())
			{
				this->legacy_backend->bindShaderProgram(this->color_point);
			}
			else
			{
				this->legacy_backend->bindShaderProgram(this->texture_point);
			}

			//Environment
			writeEnvironmentUniform(this->legacy_backend, environment);

			//Material
			writeMaterialUniform(this->legacy_backend, material_component.material);

			//Matrices
			writeTransformUniform(this->legacy_backend, render_transform);

			//Light
			writePointLightUniform(this->legacy_backend, light, light_transform);

			this->legacy_backend->draw(mesh_component.mesh->vertex_buffer, mesh_component.mesh->index_buffer, mesh_component.mesh->index_count);
		}
	}
}

void LegacyMeshRenderer::drawSpotPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, SpotLight& light, TransformD& light_transform)
{
	auto& view = entity_registry->view<LegacyMeshComponent, LegacyMaterialComponent, TransformD>();
	for (EntityHandle entity : view)
	{
		LegacyMeshComponent& mesh_component = view.get<LegacyMeshComponent>(entity);
		LegacyMaterialComponent& material_component = view.get<LegacyMaterialComponent>(entity);
		TransformD& render_transform = view.get<TransformD>(entity);

		if (frustum->sphereTest((vector3F)render_transform.getPosition(), mesh_component.mesh->frustum_sphere_radius))
		{
			if (material_component.material->texture_names[0].empty())
			{
				this->legacy_backend->bindShaderProgram(this->color_spot);
			}
			else
			{
				this->legacy_backend->bindShaderProgram(this->texture_spot);
			}

			//Environment
			writeEnvironmentUniform(this->legacy_backend, environment);

			//Material
			writeMaterialUniform(this->legacy_backend, material_component.material);

			//Matrices
			writeTransformUniform(this->legacy_backend, render_transform);

			//Light
			writeSpotLightUniform(this->legacy_backend, light, light_transform);

			this->legacy_backend->draw(mesh_component.mesh->vertex_buffer, mesh_component.mesh->index_buffer, mesh_component.mesh->index_count);
		}
	}
}