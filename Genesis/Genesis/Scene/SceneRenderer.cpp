#include "SceneRenderer.hpp"

#include "Genesis/Debug/Assert.hpp"
#include "Genesis/Scene/Frustum.hpp"

#include "Genesis/Rendering/ResourceLoaders.hpp"

using namespace Genesis;

SceneRenderer::SceneRenderer(RenderingBackend* backend)
	:RenderLayer(backend)
{
	this->view_size = this->backend->getScreenSize();

	List<ImageFormat> color(1);
	color[0] = ImageFormat::RGBA_8_Unorm;
	this->layout = FramebufferLayout(color, ImageFormat::D_16_Unorm);

	this->framebuffer = this->backend->createFramebuffer(this->layout, this->view_size);
	this->mt_command_buffer = this->backend->createMTCommandBuffer();

	this->scene_uniform_buffer = this->backend->createDynamicBuffer(sizeof(SceneUniform), BufferUsage::Uniform_Buffer, MemoryType::CPU_Visable);

	this->mesh_shader = ShaderLoader::loadShaderSingle(this->backend, "res/shaders_glsl/Model");

	{
		MaterialValues values;
		values.albedo = vector4F(1.0f, 0.1f, 0.7f, 1.0f);
		values.metallic = 0.0f;
		values.roughness = 0.0f;
		values.ambient_occlusion = 0.0f;
		values.height_scale = 0.0f;
		values.has_albedo_map = true;

		this->material_buffer = this->backend->createDynamicBuffer(sizeof(MaterialValues), BufferUsage::Uniform_Buffer, MemoryType::GPU_Only);
		this->backend->writeDynamicBuffer(this->material_buffer, &values, sizeof(MaterialValues));

		vector2U size(1, 1);
		vector<uint8_t> data(size.x * size.y * 4, 0);
		this->empty_texture = this->backend->createTexture(size, data.data(), sizeof(uint8_t) * data.size());

		SamplerCreateInfo info = {};
		this->basic_sampler = this->backend->createSampler(info);

		this->temp_texture = PngLoader::loadTexture(this->backend, "res/1K_Grid.png");
	}
}

SceneRenderer::~SceneRenderer()
{
	this->backend->destroyDynamicBuffer(this->material_buffer);
	this->backend->destroyTexture(this->empty_texture);
	this->backend->destroyTexture(this->temp_texture);

	this->backend->destroyShader(this->mesh_shader);

	this->backend->destroyFramebuffer(this->framebuffer);
	this->backend->destroyMTCommandBuffer(this->mt_command_buffer);

	this->backend->destroyDynamicBuffer(this->scene_uniform_buffer);
}

void SceneRenderer::startLayer()
{
	this->draw_call_count = 0;

	vector2U temp_size = this->backend->getScreenSize();
	if (temp_size != this->view_size)
	{
		this->view_size = temp_size;
		this->backend->resizeFramebuffer(this->framebuffer, this->view_size);
	}

	List<CommandBufferInterface*>& command_buffers = *this->backend->beginMTCommandBuffer(this->mt_command_buffer, this->framebuffer);
	this->command_buffer = command_buffers[0];
}

void SceneRenderer::endLayer()
{
	this->backend->endMTCommandBuffer(this->mt_command_buffer);
}

#include "imgui.h"
void SceneRenderer::ImGuiDraw()
{	
	ImGui::Begin("SceneRenderer");
	ImGui::LabelText(std::to_string(this->draw_call_count).c_str(), "Draw Calls");
	ImGui::Checkbox("Use Frustrum Culling", &this->use_frustum_culling);
	ImGui::End();
}

void SceneRenderer::drawScene(Scene* scene)
{
	//DRAW SCENE STEPS
	//STEP 1: DATA EXTRACTION (only place ECS should be accessed*) (MT)
	//STEP 2: CREATE VIEW LIST (figure out all veiws that need to be rendered) (ST)
	//STEP 3: CULL OBJECTS AND RENDER (MT)

	GENESIS_ENGINE_ASSERT_ERROR((scene != nullptr), "Null Scene");

	float aspect_ratio = ((float)this->view_size.x) / ((float)this->view_size.y);

	Camera camera = scene->camera.camera;
	TransformF camera_transform = scene->camera.transform;
	matrix4F view_projection_matrix = camera.getProjectionMatrix(aspect_ratio) * camera_transform.getViewMatirx();

	{
		//Update Scene Uniform
		SceneUniform temp_uniform;
		temp_uniform.camera_position = camera_transform.getPosition();
		temp_uniform.ambient_light = scene->ambient_light;
		temp_uniform.view_projection_matrix = view_projection_matrix;

		if ((this->scene_uniform.camera_position != temp_uniform.camera_position) || (this->scene_uniform.ambient_light != temp_uniform.ambient_light) || (this->scene_uniform.view_projection_matrix != temp_uniform.view_projection_matrix))
		{
			this->scene_uniform = temp_uniform;
			this->backend->writeDynamicBuffer(this->scene_uniform_buffer, &this->scene_uniform, sizeof(SceneUniform));
		}
	}

	PipelineSettings pipeline_settings;
	this->command_buffer->setPipelineSettings(pipeline_settings);
	this->command_buffer->setShader(this->mesh_shader);
	
	this->command_buffer->setUniformBuffer(0, 0, this->scene_uniform_buffer);

	//Material
	this->command_buffer->setUniformBuffer(1, 0, this->material_buffer);
	this->command_buffer->setUniformTexture(1, 1, this->temp_texture, this->basic_sampler);
	this->command_buffer->setUniformTexture(1, 2, this->empty_texture, this->basic_sampler);
	this->command_buffer->setUniformTexture(1, 3, this->empty_texture, this->basic_sampler);
	this->command_buffer->setUniformTexture(1, 4, this->empty_texture, this->basic_sampler);
	this->command_buffer->setUniformTexture(1, 5, this->empty_texture, this->basic_sampler);
	this->command_buffer->setUniformTexture(1, 6, this->empty_texture, this->basic_sampler);

	Frustum frustum(view_projection_matrix);
	for (size_t i = 0; i < scene->meshes.size(); i++)
	{
		if (!this->use_frustum_culling || frustum.sphereTest(scene->meshes[i].transform.getPosition(), scene->meshes[i].mesh.frustum_sphere_radius))
		{
			this->draw_call_count++;

			ObjectTransformUniform matrices = {};
			matrices.model_matrix = scene->meshes[i].transform.getModelMatrix();
			matrices.normal_matrix = (glm::mat3x4)scene->meshes[i].transform.getNormalMatrix();

			this->command_buffer->setUniformConstant(&matrices, sizeof(ObjectTransformUniform));

			this->command_buffer->setVertexBuffer(scene->meshes[i].mesh.vertex_buffer, *scene->meshes[i].mesh.vertex_description);
			this->command_buffer->setIndexBuffer(scene->meshes[i].mesh.index_buffer, scene->meshes[i].mesh.index_type);
			this->command_buffer->drawIndexed(scene->meshes[i].mesh.index_count);
		}
	}

	/*
	PipelineSettings pipeline_settings;

	this->command_buffer->setPipelineSettings(pipeline_settings);
	this->command_buffer->setShader(nullptr);

	this->command_buffer->setUniformBuffer(0, 0, this->scene_uniform_buffer);

	for (size_t i = 0; i < scene->meshes.size(); i++)
	{
		Scene::MeshTransform mesh = scene->meshes[i];
		if (mesh.mesh != nullptr && mesh.transform != nullptr)
		{
			ObjectTransformUniform object_transform;
			object_transform.model_matrix = mesh.transform->getModelMatrix();
			this->command_buffer->setUniformConstant(&object_transform, sizeof(ObjectTransformUniform));

			this->command_buffer->setIndexBuffer(mesh.mesh->index_buffer, mesh.mesh->index_type);
			this->command_buffer->setVertexBuffer(mesh.mesh->vertex_buffer, *mesh.mesh->vertex_description);
			this->command_buffer->drawIndexed(mesh.mesh->index_count);
		}
	}*/
}
