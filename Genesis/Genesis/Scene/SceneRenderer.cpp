#include "SceneRenderer.hpp"

#include "Genesis/Debug/Assert.hpp"

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

	this->scene_uniform_buffer = this->backend->createUniformBuffer(sizeof(SceneUniform), MemoryType::CPU_Visable);
}

SceneRenderer::~SceneRenderer()
{
	this->backend->destroyFramebuffer(this->framebuffer);
	this->backend->destroyMTCommandBuffer(this->mt_command_buffer);

	this->backend->destroyUniformBuffer(this->scene_uniform_buffer);
}

void SceneRenderer::startLayer()
{
	vector2U temp_size = this->backend->getScreenSize();
	if (temp_size != this->view_size)
	{
		this->view_size = temp_size;
		this->backend->resizeFramebuffer(this->framebuffer, this->view_size);
	}

	List<CommandBuffer*>& command_buffers = *this->backend->beginMTCommandBuffer(this->mt_command_buffer, this->framebuffer);
	this->command_buffer = command_buffers[0];
}

void SceneRenderer::endLayer()
{
	this->backend->endMTCommandBuffer(this->mt_command_buffer);
}

void SceneRenderer::drawScene(Scene* scene)
{
	//DRAW SCENE STEPS
	//STEP 1: DATA EXTRACTION (only place ECS should be accessed*) (MT)
	//STEP 2: CREATE VIEW LIST (figure out all veiws that need to be rendered) (ST)
	//STEP 3: CULL OBJECTS AND RENDER (MT)

	GENESIS_ENGINE_ASSERT_ERROR((scene != nullptr), "Null Scene");
	GENESIS_ENGINE_ASSERT_ERROR((scene->camera.camera != nullptr), "Null Camera");
	GENESIS_ENGINE_ASSERT_ERROR((scene->camera.transform != nullptr), "Null Camera Transform");

	float aspect_ratio = ((float)this->view_size.x) / ((float)this->view_size.y);

	Camera* camera = scene->camera.camera;
	TransformF* camera_transform = scene->camera.transform;
	matrix4F view_projection_matrix = camera_transform->getViewMatirx() * camera->getProjectionMatrix(aspect_ratio);

	//Update Scene Uniform
	SceneUniform temp_uniform;
	temp_uniform.camera_position = camera_transform->getPosition();
	temp_uniform.ambient_light = scene->ambient_light;
	temp_uniform.view_projection_matrix = view_projection_matrix;

	if ((this->scene_uniform.camera_position != temp_uniform.camera_position) || (this->scene_uniform.ambient_light != temp_uniform.ambient_light) || (this->scene_uniform.view_projection_matrix != temp_uniform.view_projection_matrix))
	{
		this->scene_uniform = temp_uniform;
		this->backend->setUniform(this->scene_uniform_buffer, &this->scene_uniform, sizeof(SceneUniform));
	}

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
	}
}
