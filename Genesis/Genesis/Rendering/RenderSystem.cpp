#include "RenderSystem.hpp"

#include "Genesis/Debug/Assert.hpp"
#include "Genesis/Rendering/ResourceLoaders.hpp"

using namespace Genesis;

RenderSystem::RenderSystem(RenderingBackend* backend)
{
	this->backend = backend;

	this->screen_shader = ShaderLoader::loadShaderSingle(this->backend, "res/temp_shaders/test");

	const vector<vector2F> vertices =
	{
		//Pos        UV
		{-1.0f, -1.0f}, {0.0f, 0.0f},
		{-1.0f, 1.0f}, {0.0f, 1.0f},
		{1.0f, -1.0f}, {1.0f, 0.0f},
		{1.0f, 1.0f}, {1.0f, 1.0f},
	};

	const vector<uint16_t> indices =
	{
		0, 1, 2, //First Tri
		3, 2, 1 // Second Tri
	};

	this->vertex_description = this->backend->createVertexInputDescription({ VertexElementType::float_2, VertexElementType::float_2 });

	this->screen_vertex = this->backend->createStaticBuffer((void*)vertices.data(), vertices.size() * sizeof(vector2F), BufferUsage::Vertex_Buffer);
	this->screen_index = this->backend->createStaticBuffer((void*)indices.data(), indices.size() * sizeof(uint16_t), BufferUsage::Index_Buffer);
	SamplerCreateInfo sampler_create_info = {};
	sampler_create_info.min_filter = FilterMode::Nearest;
	sampler_create_info.mag_filter = FilterMode::Nearest;
	sampler_create_info.mipmap_mode = FilterMode::Nearest;
	sampler_create_info.U_address_mode = AddressMode::Clamp_Border;
	sampler_create_info.V_address_mode = AddressMode::Clamp_Border;
	sampler_create_info.W_address_mode = AddressMode::Clamp_Border;
	sampler_create_info.mip_lod_bias = 0.0f;
	sampler_create_info.max_anisotropy = 0;
	sampler_create_info.compare_op = CompareOp::Never;
	sampler_create_info.min_lod = 0.0f;
	sampler_create_info.max_lod = 0.0f;
	sampler_create_info.border_color = BorderColor::Transparent_Black;
	this->screen_sampler = this->backend->createSampler(sampler_create_info);

	screen_settings.cull_mode = CullMode::None;
	screen_settings.depth_test = DepthTest::None;
	screen_settings.blend_op = BlendOp::Add;
	screen_settings.src_factor = BlendFactor::Alpha_Src;
	screen_settings.dst_factor = BlendFactor::One_Minus_Alpha_Src;
}

RenderSystem::~RenderSystem()
{
	this->backend->destroyStaticBuffer(this->screen_vertex);
	this->backend->destroyStaticBuffer(this->screen_index);
	this->backend->destroyShader(this->screen_shader);
}

bool RenderSystem::startFrame()
{
	this->command_buffer = this->backend->beginFrame();
	if (this->command_buffer == nullptr)
	{
		return false;
	}

	command_buffer->setPipelineSettings(screen_settings);
	command_buffer->setShader(this->screen_shader);

	command_buffer->setVertexBuffer(this->screen_vertex, this->vertex_description);
	command_buffer->setIndexBuffer(this->screen_index, IndexType::uint16);

	return true;
}

void RenderSystem::endFrame()
{
	GENESIS_ENGINE_ASSERT_ERROR((this->command_buffer != nullptr), "Command buffer not valid");
	this->backend->endFrame();
	CommandBuffer* command_buffer = nullptr;
}

void RenderSystem::drawLayerWholeScreen(RenderLayer* layer)
{
	GENESIS_ENGINE_ASSERT_ERROR((this->command_buffer != nullptr), "Command buffer not valid");
	command_buffer->setUniformFramebuffer(0, 0, layer->getFramebuffer(), layer->getFramebufferIndex(), this->screen_sampler);
	command_buffer->drawIndexed(6, 0, 1, 0);
}
