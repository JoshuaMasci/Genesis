#include "Genesis/Entity/WorldRenderer.hpp"

#include "Genesis/Debug/Assert.hpp"
#include "Genesis/Debug/Profiler.hpp"

#include "Genesis/Core/Transform.hpp"
#include "Genesis/Rendering/Frustum.hpp"
#include "Genesis/Rendering/ResourceLoaders.hpp"
#include "Genesis/Entity/MeshComponent.hpp"
#include "Genesis/Rendering/Camera.hpp"

using namespace Genesis;

WorldRenderer::WorldRenderer(RenderingBackend* backend)
:RenderLayer(backend)
{
	this->view_size = this->backend->getScreenSize();

	List<ImageFormat> color(1);
	color[0] = ImageFormat::RGBA_8_Unorm;
	this->layout = FramebufferLayout(color, ImageFormat::D_16_Unorm);

	this->framebuffer = this->backend->createFramebuffer(this->layout, this->view_size);
	this->st_command_buffer = this->backend->createSTCommandBuffer();

	this->scene_uniform_buffer = this->backend->createDynamicBuffer(sizeof(SceneUniform), BufferUsage::Uniform_Buffer, MemoryType::CPU_Visable);

	this->mesh_shader = ShaderLoader::loadShaderSingle(this->backend, "res/shaders_glsl/Model");

	{
		vector2U size(1, 1);
		vector<uint8_t> data(size.x * size.y * 4, 0);
		this->empty_texture = this->backend->createTexture(size, data.data(), sizeof(uint8_t) * data.size());

		SamplerCreateInfo info = {};
		this->basic_sampler = this->backend->createSampler(info);

		//this->temp_texture = PngLoader::loadTexture(this->backend, "res/1K_Grid.png");

		MaterialValues values;
		values.albedo = vector4F(1.0f, 0.1f, 0.7f, 1.0f);
		values.metallic = 0.0f;
		values.roughness = 0.0f;
		values.ambient_occlusion = 0.0f;
		values.height_scale = 0.0f;
		this->material = Material::createMaterial(this->backend, values, this->empty_texture);
	}
}

WorldRenderer::~WorldRenderer()
{
	{
		this->backend->destroyStaticBuffer(this->material.bindings.values);
		this->backend->destroyTexture(this->empty_texture);
		//this->backend->destroyTexture(this->temp_texture);
	}

	this->backend->destroyShader(this->mesh_shader);

	this->backend->destroyFramebuffer(this->framebuffer);
	this->backend->destroySTCommandBuffer(this->st_command_buffer);

	this->backend->destroyDynamicBuffer(this->scene_uniform_buffer);
}

#include "imgui.h"
void WorldRenderer::ImGuiDraw()
{
	ImGui::Begin("SceneRenderer");
	ImGui::LabelText(std::to_string(this->draw_call_count).c_str(), "Draw Calls");
	ImGui::Checkbox("Use Frustrum Culling", &this->use_frustum_culling);
	ImGui::End();
}

void WorldRenderer::drawWorld(World* world)
{
	GENESIS_PROFILE_FUNCTION("WorldRenderer::drawWorld");

	this->draw_call_count = 0;

	vector2U temp_size = this->backend->getScreenSize();
	if (temp_size != this->view_size)
	{
		this->view_size = temp_size;
		this->backend->resizeFramebuffer(this->framebuffer, this->view_size);
	}

	CommandBufferInterface* command_buffer = this->backend->beginSTCommandBuffer(this->st_command_buffer, this->framebuffer);

	{
		float aspect_ratio = ((float)this->view_size.x) / ((float)this->view_size.y);

		EntityHandle camera = world->getCamera();
		Camera& camera_component = world->getEntityRegistry()->get<Camera>(camera);
		TransformF camera_transform = world->getEntityRegistry()->get<TransformD>(camera).toTransformF();
		matrix4F view_projection_matrix = camera_component.getProjectionMatrix(aspect_ratio) * camera_transform.getViewMatirx();

		{
			//Update Scene Uniform
			SceneUniform temp_uniform;
			temp_uniform.camera_position = camera_transform.getPosition();
			temp_uniform.ambient_light = vector3F(1.0f);
			temp_uniform.view_projection_matrix = view_projection_matrix;

			if ((this->scene_uniform.camera_position != temp_uniform.camera_position) || (this->scene_uniform.ambient_light != temp_uniform.ambient_light) || (this->scene_uniform.view_projection_matrix != temp_uniform.view_projection_matrix))
			{
				this->scene_uniform = temp_uniform;
				this->backend->writeDynamicBuffer(this->scene_uniform_buffer, &this->scene_uniform, sizeof(SceneUniform));
			}
		}

		PipelineSettings pipeline_settings;

		{
			command_buffer->setPipelineSettings(pipeline_settings);
			command_buffer->setShader(this->mesh_shader);

			command_buffer->setUniformDynamicBuffer(0, 0, this->scene_uniform_buffer);

			//Material
			command_buffer->setUniformStaticBuffer(1, 0, this->material.bindings.values);
			command_buffer->setUniformTexture(1, 1, this->empty_texture, this->basic_sampler);
			command_buffer->setUniformTexture(1, 2, this->empty_texture, this->basic_sampler);
			command_buffer->setUniformTexture(1, 3, this->empty_texture, this->basic_sampler);
			command_buffer->setUniformTexture(1, 4, this->empty_texture, this->basic_sampler);
			command_buffer->setUniformTexture(1, 5, this->empty_texture, this->basic_sampler);
			command_buffer->setUniformTexture(1, 6, this->empty_texture, this->basic_sampler);
		}

		Frustum frustum(view_projection_matrix);

		auto& view = world->getEntityRegistry()->view<MeshComponent, TransformD>();
		for (EntityHandle entity : view)
		{
			MeshComponent& mesh_component = view.get<MeshComponent>(entity);
			TransformF render_transform = view.get<TransformD>(entity).toTransformF();

			if (frustum.sphereTest(render_transform.getPosition(), mesh_component.mesh->frustum_sphere_radius))
			{
				this->draw_call_count++;

				ObjectTransformUniform matrices = {};
				matrices.model_matrix = render_transform.getModelMatrix();
				matrices.normal_matrix = (glm::mat3x4)render_transform.getNormalMatrix();

				command_buffer->setUniformConstant(&matrices, sizeof(ObjectTransformUniform));

				command_buffer->setVertexBuffer(mesh_component.mesh->vertex_buffer, *mesh_component.mesh->vertex_description);
				command_buffer->setIndexBuffer(mesh_component.mesh->index_buffer, mesh_component.mesh->index_type);
				command_buffer->drawIndexed(mesh_component.mesh->index_count);
			}
		}
	}

	this->backend->endSTCommandBuffer(this->st_command_buffer);
}
