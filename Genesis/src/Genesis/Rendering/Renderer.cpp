#include "Renderer.hpp"

#include "Genesis/Application.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/PipelineSettings.hpp"
#include "Genesis/WorldTransform.hpp"

#include <fstream>

using namespace Genesis;

Renderer::Renderer(RenderingBackend* backend)
{
	this->backend = backend;

	Array<ImageFormat> color(1);
	color[0] = ImageFormat::RGBA_8_Unorm;
	FramebufferLayout layout = FramebufferLayout(color, ImageFormat::D_32_Float);

	this->view = this->backend->createView(this->backend->getScreenSize(), layout, CommandBufferType::SingleThread);
	this->view_size = this->backend->getScreenSize();

	this->loaded_shaders["resources/shaders/Vulkan/texture_light"] = ShaderLoader::loadShaderSingle(this->backend, "resources/shaders/Vulkan/texture_light");
}

Renderer::~Renderer()
{
	this->backend->destroyView(this->view);

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

void Renderer::drawFrame(EntityRegistry& entity_registry, EntityId camera_entity)
{
	PipelineSettings model_settings;
	PipelineSettings light_settings;
	light_settings.depth_test = DepthTest::Test_Only;
	light_settings.depth_op = DepthOp::Equal;
	light_settings.blend_op = BlendOp::Add;
	light_settings.src_factor = BlendFactor::One;
	light_settings.dst_factor = BlendFactor::One;

	vector2U temp_size = this->backend->getScreenSize();
	if (temp_size != this->view_size)
	{
		this->view_size = temp_size;
		this->backend->resizeView(this->view, this->view_size);
	}


	if (!entity_registry.has<WorldTransform>(camera_entity) || !entity_registry.has<Camera>(camera_entity))
	{
		//No camera
		return;
	}

	this->backend->startView(this->view);
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

		auto lights = entity_registry.view<DirectionalLight>();
		for (auto light : lights)
		{
			auto directional_light = entity_registry.get<DirectionalLight>(light);

			Shader light_shader = this->loaded_shaders["resources/shaders/Vulkan/texture_light"];
			command_buffer->setShader(light_shader);
			command_buffer->setUniformTexture("albedo_texture", texture);
			command_buffer->setUniformMat4("matrices.mvp", mvp);
			command_buffer->setUniformMat4("matrices.model", model_matrix);
			command_buffer->setUniformMat3("matrices.normal", normal_matrix);

			//Lights
			command_buffer->setUniformVec3("light.color", directional_light.color);
			command_buffer->setUniformFloat("light.intensity", directional_light.intensity);
			command_buffer->setUniformVec3("light.direction", glm::normalize(directional_light.direction));

			command_buffer->drawIndexed(mesh.vertex_buffer, mesh.index_buffer);
		}
	}

	this->backend->endView(this->view);
	this->backend->sumbitView(this->view);
}
