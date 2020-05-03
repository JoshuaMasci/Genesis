#include "Genesis/LegacyRendering/LegacyWorldRenderer.hpp"

#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Lighting.hpp"

#include "Genesis/Platform/FileSystem.hpp"

using namespace Genesis;

void writeEnvironmentUniform(LegacyBackend* backend, EnvironmentData* environment)
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

void writeTransformUniform(LegacyBackend* backend, const TransformF& transform)
{
	backend->setUniformMat4f("matrices.model", transform.getModelMatrix());
	backend->setUniformMat3f("matrices.normal", transform.getNormalMatrix());
}

void writeDirectionalLightUniform(LegacyBackend* backend, const DirectionalLight& light, const TransformF& light_transform)
{
	backend->setUniform3f("directional_light.base.color", light.color);
	backend->setUniform1f("directional_light.base.intensity", light.intensity);
	backend->setUniform3f("directional_light.direction", light_transform.getForward());
}

void writePointLightUniform(LegacyBackend* backend, const PointLight& light, const TransformF& light_transform)
{
	backend->setUniform3f("point_light.base.color", light.color);
	backend->setUniform1f("point_light.base.intensity", light.intensity);
	backend->setUniform1f("point_light.range", light.range);
	backend->setUniform2f("point_light.attenuation", light.attenuation);
	backend->setUniform3f("point_light.position", light_transform.getPosition());
}

void writeSpotLightUniform(LegacyBackend* backend, const SpotLight& light, const TransformF& light_transform)
{
	backend->setUniform3f("spot_light.base.color", light.color);
	backend->setUniform1f("spot_light.base.intensity", light.intensity);
	backend->setUniform1f("spot_light.range", light.range);
	backend->setUniform2f("spot_light.attenuation", light.attenuation);
	backend->setUniform3f("spot_light.position", light_transform.getPosition());
	backend->setUniform3f("spot_light.direction", light_transform.getForward());
	backend->setUniform1f("spot_light.cutoff", light.cutoff);
}

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

LegacyWorldRenderer::~LegacyWorldRenderer()
{
	delete this->material_pool;
	delete this->texture_pool;
	delete this->mesh_pool;
}

void LegacyWorldRenderer::addMesh(EntityRegistry* registry, EntityHandle entity, const string& mesh_file, const string& material_file)
{
	registry->assign<LegacyMeshComponent>(entity, this->mesh_pool->getResource(mesh_file), this->material_pool->getResource(material_file));
}

void LegacyWorldRenderer::removeEntity(EntityRegistry* registry, EntityHandle entity)
{
	//TODO unload
	registry->remove<LegacyMeshComponent>(entity);
}

void LegacyWorldRenderer::drawWorld(World* world, vector2U size)
{
	float aspect_ratio = ((float)size.x) / ((float)size.y);

	EntityHandle camera = world->getCamera();
	Camera& camera_component = world->getEntityRegistry()->get<Camera>(camera);
	TransformF camera_transform = world->getEntityRegistry()->get<TransformD>(camera).toTransformF();
	matrix4F view_projection_matrix = camera_component.getProjectionMatrix(aspect_ratio) * camera_transform.getViewMatirx();

	Frustum frustum(view_projection_matrix);
	EnvironmentData environment = { camera_transform.getPosition(), vector3F(0.1f), view_projection_matrix };

	PipelineSettings settings;
	settings.cull_mode = CullMode::Back;
	settings.depth_test = DepthTest::Test_And_Write;
	settings.depth_op = DepthOp::Less;
	settings.blend_op = BlendOp::None;
	settings.src_factor = BlendFactor::One;
	settings.dst_factor = BlendFactor::Zero;
	this->drawAmbient(world->getEntityRegistry(), &environment, &frustum);

	PipelineSettings light_settings;
	light_settings.cull_mode = CullMode::Back;
	light_settings.depth_test = DepthTest::Test_Only;
	light_settings.depth_op = DepthOp::Equal;
	light_settings.blend_op = BlendOp::Add;
	light_settings.src_factor = BlendFactor::One;
	light_settings.dst_factor = BlendFactor::One;
	this->legacy_backend->setPipelineState(light_settings);
	this->drawDirectional(world->getEntityRegistry(), &environment, &frustum);
	this->drawPoint(world->getEntityRegistry(), &environment, &frustum);
	this->drawSpot(world->getEntityRegistry(), &environment, &frustum);
}

void LegacyWorldRenderer::drawAmbient(EntityRegistry* entity_registry, EnvironmentData* environment, Frustum* frustum)
{
	PipelineSettings settings;
	settings.cull_mode = CullMode::Back;
	settings.depth_test = DepthTest::Test_And_Write;
	settings.depth_op = DepthOp::Less;
	settings.blend_op = BlendOp::None;
	settings.src_factor = BlendFactor::One;
	settings.dst_factor = BlendFactor::Zero;
	this->legacy_backend->setPipelineState(settings);

	auto& view = entity_registry->view<LegacyMeshComponent, TransformD>();
	for (EntityHandle entity : view)
	{
		LegacyMeshComponent& mesh_component = view.get<LegacyMeshComponent>(entity);
		TransformF render_transform = view.get<TransformD>(entity).toTransformF();

		if (frustum->sphereTest(render_transform.getPosition(), mesh_component.mesh->frustum_sphere_radius))
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
			writeEnvironmentUniform(this->legacy_backend, environment);

			//Material
			writeMaterialUniform(this->legacy_backend, mesh_component.material);

			//Matrices
			writeTransformUniform(this->legacy_backend, render_transform);

			this->legacy_backend->draw(mesh_component.mesh->vertex_buffer, mesh_component.mesh->index_buffer, mesh_component.mesh->index_count);
		}
	}
}

void LegacyWorldRenderer::drawDirectional(EntityRegistry* entity_registry, EnvironmentData* environment, Frustum* frustum)
{
	auto& directional_view = entity_registry->view<DirectionalLight, TransformD>();
	for (EntityHandle light_entity : directional_view)
	{
		DirectionalLight& light = directional_view.get<DirectionalLight>(light_entity);
		TransformF light_transform = directional_view.get<TransformD>(light_entity).toTransformF();

		auto& mesh_view = entity_registry->view<LegacyMeshComponent, TransformD>();
		for (EntityHandle entity : mesh_view)
		{
			LegacyMeshComponent& mesh_component = mesh_view.get<LegacyMeshComponent>(entity);
			TransformF render_transform = mesh_view.get<TransformD>(entity).toTransformF();

			if (frustum->sphereTest(render_transform.getPosition(), mesh_component.mesh->frustum_sphere_radius))
			{
				if (mesh_component.material->texture_names[0].empty())
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
				writeMaterialUniform(this->legacy_backend, mesh_component.material);

				//Matrices
				writeTransformUniform(this->legacy_backend, render_transform);

				//Light
				writeDirectionalLightUniform(this->legacy_backend, light, light_transform);

				this->legacy_backend->draw(mesh_component.mesh->vertex_buffer, mesh_component.mesh->index_buffer, mesh_component.mesh->index_count);
			}
		}
	}
}

void LegacyWorldRenderer::drawPoint(EntityRegistry* entity_registry, EnvironmentData* environment, Frustum* frustum)
{
	auto& point_view = entity_registry->view<PointLight, TransformD>();
	for (EntityHandle light_entity : point_view)
	{
		PointLight& light = point_view.get<PointLight>(light_entity);
		TransformF light_transform = point_view.get<TransformD>(light_entity).toTransformF();

		auto& mesh_view = entity_registry->view<LegacyMeshComponent, TransformD>();
		for (EntityHandle entity : mesh_view)
		{
			LegacyMeshComponent& mesh_component = mesh_view.get<LegacyMeshComponent>(entity);
			TransformF render_transform = mesh_view.get<TransformD>(entity).toTransformF();

			if (frustum->sphereTest(render_transform.getPosition(), mesh_component.mesh->frustum_sphere_radius))
			{
				if (mesh_component.material->texture_names[0].empty())
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
				writeMaterialUniform(this->legacy_backend, mesh_component.material);

				//Matrices
				writeTransformUniform(this->legacy_backend, render_transform);

				//Light
				writePointLightUniform(this->legacy_backend, light, light_transform);

				this->legacy_backend->draw(mesh_component.mesh->vertex_buffer, mesh_component.mesh->index_buffer, mesh_component.mesh->index_count);
			}
		}
	}
}

void LegacyWorldRenderer::drawSpot(EntityRegistry* entity_registry, EnvironmentData* environment, Frustum* frustum)
{
	auto& spot_view = entity_registry->view<SpotLight, TransformD>();
	for (EntityHandle light_entity : spot_view)
	{
		SpotLight& light = spot_view.get<SpotLight>(light_entity);
		TransformF light_transform = spot_view.get<TransformD>(light_entity).toTransformF();

		auto& mesh_view = entity_registry->view<LegacyMeshComponent, TransformD>();
		for (EntityHandle entity : mesh_view)
		{
			LegacyMeshComponent& mesh_component = mesh_view.get<LegacyMeshComponent>(entity);
			TransformF render_transform = mesh_view.get<TransformD>(entity).toTransformF();

			if (frustum->sphereTest(render_transform.getPosition(), mesh_component.mesh->frustum_sphere_radius))
			{
				if (mesh_component.material->texture_names[0].empty())
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
				writeMaterialUniform(this->legacy_backend, mesh_component.material);

				//Matrices
				writeTransformUniform(this->legacy_backend, render_transform);

				//Light
				writeSpotLightUniform(this->legacy_backend, light, light_transform);

				this->legacy_backend->draw(mesh_component.mesh->vertex_buffer, mesh_component.mesh->index_buffer, mesh_component.mesh->index_count);
			}
		}
	}
}
