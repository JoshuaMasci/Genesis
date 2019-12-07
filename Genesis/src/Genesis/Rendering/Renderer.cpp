#include "Renderer.hpp"

#include "Genesis/Application.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/PipelineSettings.hpp"
#include "Genesis/WorldTransform.hpp"

#include "Genesis/Rendering/RenderUtils.hpp"

#include <fstream>

using namespace Genesis;

Renderer::Renderer(RenderingBackend* backend)
	:RenderLayer(backend),
	textured_renderer(backend),
	textured_normal_renderer(backend)
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

struct PointLightData
{
	PointLight light;
	vector3F position;
};

struct SpotLightData
{
	SpotLight light;
	vector3F position;
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

	vector<DirectionalLightData> directional_lights;
	{
		auto directional_lights_scene = entity_registry.view<DirectionalLight, WorldTransform>();
		for (auto directional : directional_lights_scene)
		{
			auto directional_light = entity_registry.get<DirectionalLight>(directional);
			auto directional_light_transform = entity_registry.get<WorldTransform>(directional);

			if (directional_light.enabled&& directional_light.intensity != 0.0)
			{
				DirectionalLightData light_data;
				light_data.light = directional_light;
				light_data.direction = directional_light_transform.current_transform.getForward();

				if (!directional_light.casts_shadows || this->used_shadows >= this->shadow_views.size())
				{
					light_data.shadow_view = nullptr;
				}
				else
				{
					light_data.shadow_view = shadow_views[this->used_shadows];
					light_data.shadow_transform = this->drawDirectionalShadowView(entity_registry, shadow_views[this->used_shadows], directional_light, directional_light_transform.current_transform);
					this->sub_views.push_back(shadow_views[this->used_shadows]);
					this->used_shadows++;
				}

				directional_lights.push_back(light_data);
			}
		}
	}

	vector<PointLightData> point_lights;
	{
		auto point_lights_scene = entity_registry.view<PointLight, WorldTransform>();
		for (auto point : point_lights_scene)
		{
			auto point_light = entity_registry.get<PointLight>(point);
			auto point_light_transform = entity_registry.get<WorldTransform>(point);

			if (point_light.enabled&& point_light.intensity != 0.0)
			{
				PointLightData light_data;
				light_data.light = point_light;
				light_data.position = point_light_transform.current_transform.getPosition();
				point_lights.push_back(light_data);
			}
		}
	}

	vector<SpotLightData> spot_lights;
	{
		auto spot_lights_scene = entity_registry.view<SpotLight, WorldTransform>();
		for (auto spot : spot_lights_scene)
		{
			auto spot_light = entity_registry.get<SpotLight>(spot);
			auto spot_light_transform = entity_registry.get<WorldTransform>(spot);

			if (spot_light.enabled&& spot_light.intensity != 0.0)
			{
				SpotLightData light_data;
				light_data.light = spot_light;
				light_data.direction = spot_light_transform.current_transform.getForward();
				light_data.position = spot_light_transform.current_transform.getPosition();

				if (!spot_light.casts_shadows || this->used_shadows >= this->shadow_views.size())
				{
					light_data.shadow_view = nullptr;
				}
				else
				{
					light_data.shadow_view = shadow_views[this->used_shadows];
					light_data.shadow_transform = this->drawSpotShadowView(entity_registry, shadow_views[this->used_shadows], spot_light, spot_light_transform.current_transform);
					this->sub_views.push_back(shadow_views[this->used_shadows]);
					this->used_shadows++;
				}

				spot_lights.push_back(light_data);
			}
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
	auto& camera_transform = entity_registry.get<WorldTransform>(camera_entity);
	Transform& current = camera_transform.current_transform;
	vector3F eye_pos = current.getPosition();

	matrix4F view = glm::lookAt(current.getPosition(), current.getPosition() + current.getForward(), current.getUp());
	matrix4F proj = this->backend->getPerspectiveMatrix(&camera, this->view);
	matrix4F view_proj_matrix = proj * view;

	{
		auto entity_model = entity_registry.view<TexturedModel, WorldTransform>();
		for (auto entity : entity_model)
		{
			auto& model = entity_model.get<TexturedModel>(entity);
			auto& transform = entity_model.get<WorldTransform>(entity);

			if (!has_value(this->loaded_meshes, model.mesh))
			{
				this->loaded_meshes[model.mesh] = ObjLoader::loadMesh(this->backend, model.mesh);
			}

			if (!has_value(this->loaded_textures, model.albedo_texture))
			{
				this->loaded_textures[model.albedo_texture] = PngLoader::loadTexture(this->backend, model.albedo_texture);
			}

			Mesh mesh = this->loaded_meshes[model.mesh];
			Texture texture = this->loaded_textures[model.albedo_texture];

			matrix4F translation = glm::translate(matrix4F(1.0F), (vector3F)(transform.current_transform.getPosition()));
			matrix4F orientation = glm::toMat4((quaternionF)transform.current_transform.getOrientation());
			matrix4F model_matrix = translation * orientation;
			matrix3F normal_matrix(orientation);
			normal_matrix = glm::inverseTranspose(normal_matrix);

			command_buffer->setPipelineSettings(model_settings);

			this->textured_renderer.drawAmbient(command_buffer, mesh, texture, view_proj_matrix, model_matrix, this->ambient_light);

			command_buffer->setPipelineSettings(light_settings);

			//Directional Lights
			for (size_t i = 0; i < directional_lights.size(); i++)
			{
				DirectionalLightData light_data = directional_lights[i];

				if (light_data.shadow_view == nullptr) //No Shadow
				{
					this->textured_renderer.drawDirectional(command_buffer, mesh, texture, view_proj_matrix, model_matrix, normal_matrix, eye_pos, light_data.light, light_data.direction);
				}
				else
				{
					this->textured_renderer.drawDirectionalShadow(command_buffer, mesh, texture, view_proj_matrix, model_matrix, normal_matrix, eye_pos, light_data.light, light_data.direction, light_data.shadow_view, light_data.shadow_transform);
				}
			}

			//Point Lights
			for (size_t i = 0; i < point_lights.size(); i++)
			{
				PointLightData light_data = point_lights[i];
				this->textured_renderer.drawPoint(command_buffer, mesh, texture, view_proj_matrix, model_matrix, normal_matrix, eye_pos, light_data.light, light_data.position);
			}

			//Spot Lights
			for (size_t i = 0; i < spot_lights.size(); i++)
			{
				SpotLightData light_data = spot_lights[i];

				if (light_data.shadow_view == nullptr) //No Shadow
				{
					this->textured_renderer.drawSpot(command_buffer, mesh, texture, view_proj_matrix, model_matrix, normal_matrix, eye_pos, light_data.light, light_data.position, light_data.direction);
				}
				else
				{
					this->textured_renderer.drawSpotShadow(command_buffer, mesh, texture, view_proj_matrix, model_matrix, normal_matrix, eye_pos, light_data.light, light_data.position, light_data.direction, light_data.shadow_view, light_data.shadow_transform);
				}
			}
		}
	}

	{
		auto entity_model = entity_registry.view<TexturedNormalModel, WorldTransform>();
		for (auto entity : entity_model)
		{
			auto& model = entity_model.get<TexturedNormalModel>(entity);
			auto& transform = entity_model.get<WorldTransform>(entity);

			if (!has_value(this->loaded_meshes, model.mesh))
			{
				this->loaded_meshes[model.mesh] = ObjLoader::loadMesh_CalcTangent(this->backend, model.mesh);
			}

			if (!has_value(this->loaded_textures, model.albedo_texture))
			{
				this->loaded_textures[model.albedo_texture] = PngLoader::loadTexture(this->backend, model.albedo_texture);
			}

			if (!has_value(this->loaded_textures, model.normal_texture))
			{
				this->loaded_textures[model.normal_texture] = PngLoader::loadTexture(this->backend, model.normal_texture);
			}

			if (!has_value(this->loaded_textures, model.height_texture))
			{
				this->loaded_textures[model.height_texture] = PngLoader::loadTexture(this->backend, model.height_texture);
			}

			Mesh mesh = this->loaded_meshes[model.mesh];
			Texture albedo_texture = this->loaded_textures[model.albedo_texture];
			Texture normal_texture = this->loaded_textures[model.normal_texture];
			Texture height_texture = this->loaded_textures[model.height_texture];

			matrix4F translation = glm::translate(matrix4F(1.0F), (vector3F)(transform.current_transform.getPosition()));
			matrix4F orientation = glm::toMat4((quaternionF)transform.current_transform.getOrientation());
			matrix4F model_matrix = translation * orientation;
			matrix3F normal_matrix(orientation);
			normal_matrix = glm::inverseTranspose(normal_matrix);

			command_buffer->setPipelineSettings(model_settings);

			this->textured_normal_renderer.drawAmbient(command_buffer, mesh, albedo_texture, height_texture, model.height_scale, view_proj_matrix, model_matrix, normal_matrix, eye_pos, this->ambient_light);

			command_buffer->setPipelineSettings(light_settings);

			//Directional Lights
			for (size_t i = 0; i < directional_lights.size(); i++)
			{
				DirectionalLightData light_data = directional_lights[i];

				if (light_data.shadow_view == nullptr) //No Shadow
				{
					this->textured_normal_renderer.drawDirectional(command_buffer, mesh,  albedo_texture, normal_texture, height_texture, model.height_scale, view_proj_matrix, model_matrix, normal_matrix, eye_pos, light_data.light, light_data.direction);
				}
				else
				{
					this->textured_normal_renderer.drawDirectionalShadow(command_buffer, mesh,  albedo_texture, normal_texture, height_texture, model.height_scale, view_proj_matrix, model_matrix, normal_matrix, eye_pos, light_data.light, light_data.direction, light_data.shadow_view, light_data.shadow_transform);
				}
			}

			//Point Lights
			for (size_t i = 0; i < point_lights.size(); i++)
			{
				PointLightData light_data = point_lights[i];
				this->textured_normal_renderer.drawPoint(command_buffer, mesh,  albedo_texture, normal_texture, height_texture, model.height_scale, view_proj_matrix, model_matrix, normal_matrix, eye_pos, light_data.light, light_data.position);
			}

			//Spot Lights
			for (size_t i = 0; i < spot_lights.size(); i++)
			{
				SpotLightData light_data = spot_lights[i];

				if (light_data.shadow_view == nullptr) //No Shadow
				{
					this->textured_normal_renderer.drawSpot(command_buffer, mesh,  albedo_texture, normal_texture, height_texture, model.height_scale, view_proj_matrix, model_matrix, normal_matrix, eye_pos, light_data.light, light_data.position, light_data.direction);
				}
				else
				{
					this->textured_normal_renderer.drawSpotShadow(command_buffer, mesh,  albedo_texture, normal_texture, height_texture, model.height_scale, view_proj_matrix, model_matrix, normal_matrix, eye_pos, light_data.light, light_data.position, light_data.direction, light_data.shadow_view, light_data.shadow_transform);
				}
			}
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

matrix4F Renderer::drawDirectionalShadowView(EntityRegistry& entity_registry, View shadow_view, DirectionalLight& directional, Transform& light_transform)
{
	PipelineSettings model_settings;
	model_settings.depth_test = DepthTest::Test_And_Write;
	model_settings.depth_op = DepthOp::Less;

	matrix4F view = glm::lookAt((vector3F)light_transform.getPosition(), (vector3F)light_transform.getPosition() + (vector3F)light_transform.getForward(), (vector3F)light_transform.getUp());

	float light_x = directional.shadow_size.x / 2.0f;
	float light_y = directional.shadow_size.y / 2.0f;

	matrix4F proj = glm::ortho(light_x, -light_x, -light_y, light_y, 0.1f, 100.0f);
	proj[1][1] *= -1.0f;//TODO not hardcode this

	matrix4F view_proj_matrix = proj * view;

	this->backend->startView(shadow_view);
	CommandBuffer* command_buffer = this->backend->getViewCommandBuffer(shadow_view);
	command_buffer->setPipelineSettings(model_settings);

	{
		auto entity_model = entity_registry.view<TexturedModel, WorldTransform>();
		for (auto entity : entity_model)
		{
			auto& model = entity_model.get<TexturedModel>(entity);
			auto& transform = entity_model.get<WorldTransform>(entity);

			if (!has_value(this->loaded_meshes, model.mesh))
			{
				this->loaded_meshes[model.mesh] = ObjLoader::loadMesh(this->backend, model.mesh);
			}

			Mesh mesh = this->loaded_meshes[model.mesh];

			matrix4F translation = glm::translate(matrix4F(1.0F), (vector3F)(transform.current_transform.getPosition()));
			matrix4F orientation = glm::toMat4((quaternionF)transform.current_transform.getOrientation());
			matrix4F model_matrix = translation * orientation;

			this->textured_renderer.drawShadow(command_buffer, mesh, view_proj_matrix, model_matrix);
		}
	}

	{
		auto entity_model = entity_registry.view<TexturedNormalModel, WorldTransform>();
		for (auto entity : entity_model)
		{
			auto& model = entity_model.get<TexturedNormalModel>(entity);
			auto& transform = entity_model.get<WorldTransform>(entity);

			if (!has_value(this->loaded_meshes, model.mesh))
			{
				this->loaded_meshes[model.mesh] = ObjLoader::loadMesh_CalcTangent(this->backend, model.mesh);
			}

			Mesh mesh = this->loaded_meshes[model.mesh];

			matrix4F translation = glm::translate(matrix4F(1.0F), (vector3F)(transform.current_transform.getPosition()));
			matrix4F orientation = glm::toMat4((quaternionF)transform.current_transform.getOrientation());
			matrix4F model_matrix = translation * orientation;

			this->textured_normal_renderer.drawShadow(command_buffer, mesh, view_proj_matrix, model_matrix);
		}
	}

	this->backend->endView(shadow_view);
	this->backend->submitView(shadow_view);

	return view_proj_matrix;
}

matrix4F Renderer::drawSpotShadowView(EntityRegistry& entity_registry, View shadow_view, SpotLight& spot, Transform& light_transform)
{
	PipelineSettings model_settings;
	model_settings.depth_test = DepthTest::Test_And_Write;
	model_settings.depth_op = DepthOp::Less;

	matrix4F view = glm::lookAt((vector3F)light_transform.getPosition(), (vector3F)light_transform.getPosition() + (vector3F)light_transform.getForward(), (vector3F)light_transform.getUp());

	Camera camera(spot.cutoff);
	matrix4F proj = this->backend->getPerspectiveMatrix(&camera, shadow_view);

	matrix4F view_proj_matrix = proj * view;

	this->backend->startView(shadow_view);
	CommandBuffer* command_buffer = this->backend->getViewCommandBuffer(shadow_view);
	command_buffer->setPipelineSettings(model_settings);
	command_buffer->setShader(this->loaded_shaders["resources/shaders/vulkan/shadow"]);

	{
		auto entity_model = entity_registry.view<TexturedModel, WorldTransform>();
		for (auto entity : entity_model)
		{
			auto& model = entity_model.get<TexturedModel>(entity);
			auto& transform = entity_model.get<WorldTransform>(entity);

			if (!has_value(this->loaded_meshes, model.mesh))
			{
				this->loaded_meshes[model.mesh] = ObjLoader::loadMesh(this->backend, model.mesh);
			}

			Mesh mesh = this->loaded_meshes[model.mesh];

			matrix4F translation = glm::translate(matrix4F(1.0F), (vector3F)(transform.current_transform.getPosition()));
			matrix4F orientation = glm::toMat4((quaternionF)transform.current_transform.getOrientation());
			matrix4F model_matrix = translation * orientation;

			this->textured_renderer.drawShadow(command_buffer, mesh, view_proj_matrix, model_matrix);
		}
	}

	{
		auto entity_model = entity_registry.view<TexturedNormalModel, WorldTransform>();
		for (auto entity : entity_model)
		{
			auto& model = entity_model.get<TexturedNormalModel>(entity);
			auto& transform = entity_model.get<WorldTransform>(entity);

			if (!has_value(this->loaded_meshes, model.mesh))
			{
				this->loaded_meshes[model.mesh] = ObjLoader::loadMesh(this->backend, model.mesh);
			}

			Mesh mesh = this->loaded_meshes[model.mesh];

			matrix4F translation = glm::translate(matrix4F(1.0F), (vector3F)(transform.current_transform.getPosition()));
			matrix4F orientation = glm::toMat4((quaternionF)transform.current_transform.getOrientation());
			matrix4F model_matrix = translation * orientation;

			this->textured_normal_renderer.drawShadow(command_buffer, mesh, view_proj_matrix, model_matrix);
		}
	}

	this->backend->endView(shadow_view);
	this->backend->submitView(shadow_view);

	return view_proj_matrix;
}