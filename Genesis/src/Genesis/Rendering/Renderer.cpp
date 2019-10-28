#include "Renderer.hpp"

#include "Genesis/Application.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/PipelineSettings.hpp"
#include "Genesis/WorldTransform.hpp"

#include <fstream>

using namespace Genesis;

Renderer::Renderer(RenderingBackend* backend)
	:RenderLayer(backend)
{
	this->backend = backend;

	Array<ImageFormat> color(1);
	color[0] = ImageFormat::RGBA_8_Unorm;
	FramebufferLayout main_layout = FramebufferLayout(color, ImageFormat::D_32_Float);

	this->view = this->backend->createView(this->backend->getScreenSize(), main_layout, CommandBufferType::SingleThread);
	this->view_size = this->backend->getScreenSize();

	this->shadow_size = vector2U(1024);
	FramebufferLayout shadow_layout = FramebufferLayout(Array<ImageFormat>(), ImageFormat::D_32_Float);
	this->shadow_views = Array<View>(10);
	for (size_t i = 0; i < this->shadow_views.size(); i++)
	{
		this->shadow_views[i] = this->backend->createView(this->shadow_size, shadow_layout, CommandBufferType::SingleThread);
	}

	this->loaded_shaders["resources/shaders/vulkan/texture_light"] = ShaderLoader::loadShaderSingle(this->backend, "resources/shaders/vulkan/texture_light");
	this->loaded_shaders["resources/shaders/vulkan/shadow"] = ShaderLoader::loadShaderSingle(this->backend, "resources/shaders/vulkan/shadow");
	this->loaded_shaders["resources/shaders/vulkan/texture_light_shadow"] = ShaderLoader::loadShaderSingle(this->backend, "resources/shaders/vulkan/texture_light_shadow");
}

Renderer::~Renderer()
{
	this->backend->destroyView(this->view);

	for (size_t i = 0; i < this->shadow_views.size(); i++)
	{
		this->backend->destroyView(this->shadow_views[i]);
	}

	for (auto mesh : this->loaded_meshes)
	{
		this->backend->destroyVertexBuffer(mesh.second.vertex_buffer);
		this->backend->destroyIndexBuffer(mesh.second.index_buffer);
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

void Renderer::startFrame()
{
	vector2U temp_size = this->backend->getScreenSize();
	if (temp_size != this->view_size)
	{
		this->view_size = temp_size;
		this->backend->resizeView(this->view, this->view_size);
	}

	this->backend->startView(this->view);

	this->sub_views.clear();
	this->used_shadows = 0;
}

void Renderer::endFrame()
{
	this->backend->endView(this->view);
	this->backend->submitView(this->view, this->sub_views);
}

struct DirectionalLightData
{
	DirectionalLight light;
	vector3F direction;
	View shadow_view;
	matrix4F shadow_transform;
};

void Renderer::drawWorld(EntityRegistry& entity_registry, EntityId camera_entity)
{
	PipelineSettings model_settings;
	PipelineSettings light_settings;
	light_settings.depth_test = DepthTest::Test_Only;
	light_settings.depth_op = DepthOp::Equal;
	light_settings.blend_op = BlendOp::Add;
	light_settings.src_factor = BlendFactor::One;
	light_settings.dst_factor = BlendFactor::One;

	if (!entity_registry.has<WorldTransform>(camera_entity) || !entity_registry.has<Camera>(camera_entity))
	{
		//No camera
		return;
	}

	vector<DirectionalLightData> directional_light_shadow;

	auto lights = entity_registry.view<DirectionalLight, WorldTransform>();
	for (auto light : lights)
	{
		auto directional_light = entity_registry.get<DirectionalLight>(light);
		auto directional_light_transform = entity_registry.get<WorldTransform>(light);

		if (directional_light.enabled && directional_light.casts_shadows)
		{
			matrix4F light_transform = this->drawDirectionalShadowView(entity_registry, shadow_views[this->used_shadows], directional_light, directional_light_transform.current_transform);
			this->sub_views.push_back(shadow_views[this->used_shadows]);
			directional_light_shadow.push_back({directional_light, ((vector3F)directional_light_transform.current_transform.getForward()), shadow_views[this->used_shadows], light_transform});
			this->used_shadows++;
		}
	}

	CommandBuffer* command_buffer = this->backend->getViewCommandBuffer(this->view);

	if (command_buffer == nullptr)
	{
		return;
	}

	vector2I zero = {0,0};
	command_buffer->setScissor(zero, this->view_size);

	float aspect_ratio = ((float)this->view_size.x) / ((float)this->view_size.y);

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

		if (!has_value(this->loaded_meshes, model.mesh))
		{
			this->loaded_meshes[model.mesh] = ObjLoader::loadMesh(this->backend, model.mesh);
		}

		if (!has_value(this->loaded_textures, model.texture))
		{
			this->loaded_textures[model.texture] = PngLoader::loadTexture(this->backend, model.texture);
		}

		if (!has_value(this->loaded_shaders, model.shader))
		{
			this->loaded_shaders[model.shader] = ShaderLoader::loadShaderSingle(this->backend, model.shader);
		}

		Mesh mesh = this->loaded_meshes[model.mesh];
		Texture texture = this->loaded_textures[model.texture];
		Shader shader = this->loaded_shaders[model.shader];

		matrix4F translation = glm::translate(matrix4F(1.0F), (vector3F)(transform.current_transform.getPosition()));
		matrix4F orientation = glm::toMat4((quaternionF)transform.current_transform.getOrientation());
		matrix4F model_matrix = translation * orientation;
		matrix4F mvp = mv * model_matrix;
		matrix3F normal_matrix(orientation);
		normal_matrix = glm::inverseTranspose(normal_matrix);

		command_buffer->setShader(shader);
		command_buffer->setPipelineSettings(model_settings);
		command_buffer->setUniformTexture("albedo_texture", texture);
		command_buffer->setUniformMat4("matrices.mvp", mvp);
		command_buffer->setUniformVec3("environment.ambient_light", this->ambient_light);

		command_buffer->drawIndexed(mesh.vertex_buffer, mesh.index_buffer);

		command_buffer->setPipelineSettings(light_settings);

		//Lights
		auto lights = entity_registry.view<DirectionalLight, WorldTransform>();
		for (auto light : lights)
		{
			auto directional_light = entity_registry.get<DirectionalLight>(light);
			auto directional_light_transform = entity_registry.get<WorldTransform>(light);

			if (directional_light.enabled && !directional_light.casts_shadows)
			{
				Shader light_shader = this->loaded_shaders["resources/shaders/vulkan/texture_light"];
				command_buffer->setShader(light_shader);
				command_buffer->setUniformTexture("albedo_texture", texture);
				command_buffer->setUniformMat4("matrices.mvp", mvp);
				command_buffer->setUniformMat4("matrices.model", model_matrix);
				command_buffer->setUniformMat3("matrices.normal", normal_matrix);

				//Light
				command_buffer->setUniformVec3("lights.directional.color", directional_light.color);
				command_buffer->setUniformFloat("lights.directional.intensity", directional_light.intensity);
				command_buffer->setUniformVec3("lights.directional.direction", directional_light_transform.current_transform.getForward());
				command_buffer->setUniformVec3("lights.eye_pos", (vector3F)transform.current_transform.getPosition());
				
				command_buffer->drawIndexed(mesh.vertex_buffer, mesh.index_buffer);
			}
		}

		//Lights with shadows
		for (size_t i = 0; i < directional_light_shadow.size(); i++)
		{
			DirectionalLightData& data = directional_light_shadow[i];

			Shader light_shader = this->loaded_shaders["resources/shaders/vulkan/texture_light_shadow"];
			command_buffer->setShader(light_shader);
			command_buffer->setUniformTexture("albedo_texture", texture);
			command_buffer->setUniformMat4("matrices.mvp", mvp);
			command_buffer->setUniformMat4("matrices.model", model_matrix);
			command_buffer->setUniformMat3("matrices.normal", normal_matrix);

			//Light
			command_buffer->setUniformVec3("lights.directional.color", data.light.color);
			command_buffer->setUniformFloat("lights.directional.intensity", data.light.intensity);
			command_buffer->setUniformVec3("lights.directional.direction", data.direction);
			command_buffer->setUniformVec3("lights.eye_pos", (vector3F)transform.current_transform.getPosition());
			command_buffer->setUniformMat4("lights.shadow_mv", data.shadow_transform);
			command_buffer->setUniformView("shadow_map", data.shadow_view, 0);

			command_buffer->drawIndexed(mesh.vertex_buffer, mesh.index_buffer);
		}
	}
}

void Renderer::setScreenSize(vector2U size)
{
	this->backend->resizeView(this->view, size);
}

View Renderer::getView()
{
	return this->view;
}

uint32_t Renderer::getViewImageIndex()
{
	return 0;
}

matrix4F Renderer::drawDirectionalShadowView(EntityRegistry& entity_registry, View shadow_view, DirectionalLight& light, Transform& light_transform)
{
	PipelineSettings model_settings;
	model_settings.depth_test = DepthTest::Test_And_Write;
	model_settings.depth_op = DepthOp::Less;

	matrix4F view = glm::lookAt((vector3F)light_transform.getPosition(), (vector3F)light_transform.getPosition() + (vector3F)light_transform.getForward(), (vector3F)light_transform.getUp());

	float light_x = light.shadow_size.x / 2.0f;
	float light_y = light.shadow_size.y / 2.0f;

	Camera camera(20.0f);
	matrix4F proj = glm::ortho(light_x, -light_x, -light_y, light_y, 0.1f, 100.0f);
	proj[1][1] *= -1.0f;//TODO not hardcode this

	matrix4F mv = proj * view;

	this->backend->startView(shadow_view);
	CommandBuffer* command_buffer = this->backend->getViewCommandBuffer(shadow_view);
	command_buffer->setPipelineSettings(model_settings);
	command_buffer->setShader(this->loaded_shaders["resources/shaders/vulkan/shadow"]);

	auto entity_model = entity_registry.view<Model, WorldTransform>();
	for (auto entity : entity_model)
	{
		auto &model = entity_model.get<Model>(entity);
		auto &transform = entity_model.get<WorldTransform>(entity);

		if (!has_value(this->loaded_meshes, model.mesh))
		{
			this->loaded_meshes[model.mesh] = ObjLoader::loadMesh(this->backend, model.mesh);
		}

		if (!has_value(this->loaded_textures, model.texture))
		{
			this->loaded_textures[model.texture] = PngLoader::loadTexture(this->backend, model.texture);
		}

		Mesh mesh = this->loaded_meshes[model.mesh];

		matrix4F translation = glm::translate(matrix4F(1.0F), (vector3F)(transform.current_transform.getPosition()));
		matrix4F orientation = glm::toMat4((quaternionF)transform.current_transform.getOrientation());
		matrix4F model_matrix = translation * orientation;
		matrix4F mvp = mv * model_matrix;
		command_buffer->setUniformMat4("matrices.mvp", mvp);

		command_buffer->drawIndexed(mesh.vertex_buffer, mesh.index_buffer);
	}

	this->backend->endView(shadow_view);
	this->backend->submitView(shadow_view);

	return mv;
}
