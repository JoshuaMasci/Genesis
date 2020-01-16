#include "GameScene.hpp"

#include "Genesis/Application.hpp"
#include "Genesis/WorldTransform.hpp"
#include "Genesis/Rendering/Renderer.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/DebugCamera.hpp"
#include "Genesis/Rendering/Model.hpp"

#include <fstream>

using namespace Genesis;

GameScene::GameScene(Application* app)
{
	this->application = app;

	this->renderer = new Renderer(this->application->rendering_backend);

	this->screen_shader = ShaderLoader::loadShaderSingle(this->application->rendering_backend, "res/temp_shaders/test");

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

	VertexInputDescription vertex_description
	({
		{"in_position", VertexElementType::float_2},
		{"in_uv", VertexElementType::float_2},
	});

	this->screen_vertex = this->application->rendering_backend->createVertexBuffer((void*)vertices.data(), vertices.size() * sizeof(vector2F), vertex_description);
	this->screen_index = this->application->rendering_backend->createIndexBuffer((void*)indices.data(), indices.size() * sizeof(uint16_t), IndexType::uint16);

	this->ui_renderer = new ImGuiRenderer(this->application->rendering_backend, &this->application->input_manager);

	this->screen_sampler.min_filter = FilterMode::Nearest;
	this->screen_sampler.mag_filter = FilterMode::Nearest;
	this->screen_sampler.mipmap_mode = FilterMode::Nearest;
	this->screen_sampler.U_address_mode = AddressMode::Clamp_Border;
	this->screen_sampler.V_address_mode = AddressMode::Clamp_Border;
	this->screen_sampler.W_address_mode = AddressMode::Clamp_Border;
	this->screen_sampler.mip_lod_bias = 0.0f;
	this->screen_sampler.max_anisotropy = 0;
	this->screen_sampler.compare_op = CompareOp::Never;
	this->screen_sampler.min_lod = 0.0f;
	this->screen_sampler.max_lod = 0.0f;
	this->screen_sampler.border_color = BorderColor::Transparent_Black;

	/*{
		this->mesh = AssimpLoader::loadMesh(this->application->rendering_backend, "res/temp1.fbx");
		this->skeleton = AssimpLoader::loadSkeleton(this->application->rendering_backend, "res/temp1.fbx");

		for (size_t i = 0; i < this->skeleton->getBoneCount(); i++)
		{
			Bone* bone = this->skeleton->getBone(i);

			if (i == 0)
			{
				bone->local_transform = TransformF();
			}
			else
			{
				bone->local_transform = TransformF(vector3F(0.0f, 0.5f, 0.0f));
			}
			vector3F pos = vector3F(0.0f, 0.5f * (float)i, 0.0f);
			TransformF global_transform = TransformF(pos);
			bone->inverse_bind_transform = glm::inverse(global_transform.calcMatrix4F());
		}
		this->skeleton->getBone(1)->local_transform.setScale(vector3F(0.5f));


		this->mesh_shader = ShaderLoader::loadShaderSingle(this->application->rendering_backend, "res/temp_shaders/AnimatedModel");
		this->mesh_description = AnimatedModel::getVertexDescription();
		this->texture = PngLoader::loadTexture(this->application->rendering_backend, "res/Colors.png");
	}

	this->bones = this->application->rendering_backend->createUniformBuffer(sizeof(matrix4F) * 64);*/
}

GameScene::~GameScene()
{
	if (this->renderer != nullptr)
	{
		delete this->renderer;
	}

	if (this->ui_renderer != nullptr)
	{
		delete this->ui_renderer;
	}

	this->application->rendering_backend->destroyVertexBuffer(this->screen_vertex);
	this->application->rendering_backend->destroyIndexBuffer(this->screen_index);
	this->application->rendering_backend->destroyShader(this->screen_shader);
}

void GameScene::runSimulation(double delta_time)
{

}

#include "imgui.h"

void GameScene::drawWorld(double delta_time)
{
	CommandBuffer* command_buffer = this->application->rendering_backend->beginFrame();
	if (command_buffer != nullptr)
	{
		/*Bone* bone = this->skeleton->getBone("Bone.001");
		//bone->local_animated_transform = glm::rotate(bone->local_animated_transform, glm::radians(20.0f * (float)delta_time), vector3F(0.0f, 1.0f, 0.0f));

		static float scale = 1.0f;
		this->skeleton->getBone(1)->local_transform.setScale(vector3F(scale));
		scale -= 0.05f * (float)delta_time;

		List<matrix4F> bone_mat(this->skeleton->getBoneCount());
		this->skeleton->calcBoneMatrices(bone_mat.data());
		this->application->rendering_backend->setUniform(this->bones, bone_mat.data(), sizeof(matrix4F) * bone_mat.size());

		PipelineSettings mesh_settings;
		mesh_settings.cull_mode = CullMode::None;

		command_buffer->setPipelineSettings(mesh_settings);
		command_buffer->setShader(this->mesh_shader);
		command_buffer->setUniformConstant(&mvp[0][0], sizeof(matrix4F));
		command_buffer->setUniformBuffer(0, 0, this->bones);
		Sampler sampler = {};
		command_buffer->setUniformTexture(0, 1, this->texture, sampler);
		command_buffer->setVertexBuffer(this->mesh.vertex_buffer, this->mesh_description);
		command_buffer->setIndexBuffer(this->mesh.index_buffer, IndexType::uint32);
		command_buffer->drawIndexed(this->mesh.index_count);*/

		this->ui_renderer->startLayer();
		
		ImGui::Begin("Scale");
		ImGui::SliderFloat("Scale", &this->renderer->scale, 0.0f, 3.0f);
		ImGui::End();

		this->ui_renderer->endLayer();

		this->renderer->startLayer();
		this->renderer->endLayer();

		PipelineSettings screen_settings;
		screen_settings.cull_mode = CullMode::None;
		screen_settings.depth_test = DepthTest::None;
		screen_settings.blend_op = BlendOp::Add;
		screen_settings.src_factor = BlendFactor::Alpha_Src;
		screen_settings.dst_factor = BlendFactor::One_Minus_Alpha_Src;

		command_buffer->setPipelineSettings(screen_settings);
		command_buffer->setShader(this->screen_shader);

		command_buffer->setVertexBuffer(this->screen_vertex, VertexInputDescription());
		command_buffer->setIndexBuffer(this->screen_index, IndexType::uint16);

		command_buffer->setUniformView(0, 0, this->renderer->getView(), this->renderer->getViewImageIndex(), this->screen_sampler);
		command_buffer->drawIndexed(6, 0, 1, 0);

		command_buffer->setUniformView(0, 0, this->ui_renderer->getView(), this->ui_renderer->getViewImageIndex(), this->screen_sampler);
		command_buffer->drawIndexed(6, 0, 1, 0);

		this->application->rendering_backend->endFrame();
	}
}
