#include "Renderer.hpp"

using namespace Genesis;

Renderer::Renderer(RenderingBackend* backend)
	:RenderLayer(backend)
{
	List<ImageFormat> color(1);
	color[0] = ImageFormat::RGBA_8_Unorm;
	this->layout = FramebufferLayout(color, ImageFormat::D_16_Unorm);
	this->view = this->backend->createView(this->layout, this->backend->getScreenSize());

	this->shader = ShaderLoader::loadShaderSingle(this->backend, "res/temp_shaders/Model");
	this->mesh = ObjLoader::loadMesh(this->backend, "res/cube.obj");
}

Renderer::~Renderer()
{
	this->backend->destroyView(this->view);

	this->backend->destroyVertexBuffer(this->mesh.vertex_buffer);
	this->backend->destroyIndexBuffer(this->mesh.index_buffer);
	this->backend->destroyShader(this->shader);
}

void Renderer::startLayer()
{
	vector2U temp_size = this->backend->getScreenSize();
	if (temp_size != this->view_size)
	{
		this->view_size = temp_size;
		this->backend->resizeView(this->view, this->view_size);
	}

	this->command_buffer = this->backend->beginView(this->view);
}

void Renderer::endLayer()
{
	{
		vector2U size = this->backend->getScreenSize();
		float aspect_ratio = ((float)size.x) / ((float) size.y);
		float fovy = atan(tan(glm::radians(77.0f) / 2.0f) / aspect_ratio) * 2.0f;
		matrix4F view = glm::lookAt(vector3F(0.0f, 0.0f, -5.0f), vector3F(0.0f), vector3F(0.0f, 1.0f, 0.0f));
		matrix4F proj = glm::infinitePerspective(fovy, aspect_ratio, 0.1f);

		matrix4F view_projection = proj * view;

		TransformF model_transform;
		model_transform.setPosition(vector3F(0.0f, 0.5f, 0.0f));
		model_transform.setScale(vector3F(this->scale));
		matrix4F model = model_transform.calcMatrix4F();

		ColorUniform uniform = { vector4F(1.0f, 0.0f, 1.0f, 1.0f), view_projection, model };

		PipelineSettings mesh_settings;
		mesh_settings.cull_mode = CullMode::None;
		this->drawModel(this->shader, mesh_settings, this->mesh, &uniform, (uint32_t)sizeof(ColorUniform));
	}

	assert(this->command_buffer != nullptr);

	this->backend->endView(this->view);
	this->command_buffer = nullptr;
}

View Renderer::getView()
{
	return this->view;
}

uint32_t Renderer::getViewImageIndex()
{
	return 0;
}

void Renderer::drawModel(Shader shader, PipelineSettings& settings, Mesh& mesh, void* uniform_const, uint32_t uniform_const_size)
{
	assert(this->command_buffer != nullptr);
	this->command_buffer->setShader(shader);
	this->command_buffer->setPipelineSettings(settings);
	if(uniform_const_size > 0)
		this->command_buffer->setUniformConstant(uniform_const, uniform_const_size);
	this->command_buffer->setVertexBuffer(mesh.vertex_buffer, VertexInputDescription());
	this->command_buffer->setIndexBuffer(mesh.index_buffer, IndexType::uint32);
	this->command_buffer->drawIndexed(mesh.index_count);
}
