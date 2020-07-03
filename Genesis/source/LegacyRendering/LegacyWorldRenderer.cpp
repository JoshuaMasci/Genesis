#include "Genesis/LegacyRendering/LegacyWorldRenderer.hpp"

#include "Genesis/Platform/FileSystem.hpp"
#include "Genesis/Rendering/Camera.hpp"

#include "Genesis/Resource/PbrMesh.hpp"
#include "Genesis/Resource/PbrMaterial.hpp"

namespace Genesis
{
	LegacyWorldRenderer::LegacyWorldRenderer(LegacyBackend* backend, vector2U size)
	{
		this->backend = backend;

		//glTF.vert
		string vert_data = "";
		string frag_data = "";

		FileSystem::loadFileString("res/shaders_opengl/Model.vert", vert_data);
		FileSystem::loadFileString("res/shaders_opengl/Model.frag", frag_data);
		this->pbr_program = this->backend->createShaderProgram(vert_data.data(), (uint32_t)vert_data.size(), frag_data.data(), (uint32_t)frag_data.size());

		FileSystem::loadFileString("res/shaders_opengl/Model.vert", vert_data);
		FileSystem::loadFileString("res/shaders_opengl/Model_Window.frag", frag_data);
		this->window_program = this->backend->createShaderProgram(vert_data.data(), (uint32_t)vert_data.size(), frag_data.data(), (uint32_t)frag_data.size());

		this->framebuffers.resize(this->framebuffer_count);
		this->rebuildFramebuffers(size);
	}

	LegacyWorldRenderer::~LegacyWorldRenderer()
	{
		this->backend->destoryShaderProgram(this->window_program);
		this->backend->destoryShaderProgram(this->pbr_program);
	}

	void LegacyWorldRenderer::rebuildFramebuffers(vector2U size)
	{
		for (size_t i = 0; i < this->framebuffers.size(); i++)
		{
			this->backend->destoryFramebuffer(this->framebuffers[i]);
		}

		this->framebuffer_size = size;

		Genesis::FramebufferAttachmentInfo color_attachment = { Genesis::TextureFormat::RGBA, Genesis::MultisampleCount::Sample_1 };
		Genesis::FramebufferDepthInfo depth_attachment = { Genesis::DepthFormat::depth_24,  Genesis::MultisampleCount::Sample_1 };
		Genesis::FramebufferCreateInfo create_info = {};
		create_info.attachments = &color_attachment;
		create_info.attachment_count = 1;
		create_info.depth_attachment = &depth_attachment;
		create_info.size = this->framebuffer_size;

		for (size_t i = 0; i < this->framebuffers.size(); i++)
		{
			this->framebuffers[i] = this->backend->createFramebuffer(create_info);
		}
	}



	Framebuffer LegacyWorldRenderer::drawScene(Camera& camera, TransformD& camera_transform)
	{
		return this->framebuffers[0];
	}

	void writeMaterialUniform(LegacyBackend* backend, const PbrMaterial& material)
	{
		backend->setUniform4f("material.albedo", material.albedo_factor);
		backend->setUniform2f("material.metallic_roughness", material.metallic_roughness_factor);
		backend->setUniform4f("material.emissive", material.emissive_factor);

		backend->setUniform1i("material.albedo_uv", material.albedo_uv);
		backend->setUniform1i("material.metallic_roughness_uv", material.metallic_roughness_uv);
		backend->setUniform1i("material.normal_uv", material.normal_uv);
		backend->setUniform1i("material.occlusion_uv", material.occlusion_uv);
		backend->setUniform1i("material.emissive_uv", material.emissive_uv);

		if (material.albedo_texture != nullptr)
		{
			backend->setUniformTexture("material.albedo_texture", 0, material.albedo_texture);
		}

		if (material.metallic_roughness_texture != nullptr)
		{
			backend->setUniformTexture("material.metallic_roughness_texture", 1, material.metallic_roughness_texture);
		}

		if (material.normal_texture != nullptr)
		{
			backend->setUniformTexture("material.normal_texture", 2, material.normal_texture);
		}

		if (material.occlusion_texture != nullptr)
		{
			backend->setUniformTexture("material.occlusion_texture", 3, material.occlusion_texture);
		}

		if (material.emissive_texture != nullptr)
		{
			backend->setUniformTexture("material.emissive_texture", 4, material.emissive_texture);
		}
	}

	void writeTransformUniform(LegacyBackend* backend, TransformD& transform)
	{
		backend->setUniformMat4f("matrices.model", transform.getModelMatrix());
		//backend->setUniformMat3f("matrices.normal", transform.getNormalMatrix());
	}
}