#include "Genesis/LegacyRendering/LegacyWorldRenderer.hpp"

#include "Genesis/World/World.hpp"
#include "Genesis/Entity/Entity.hpp"

#include "Genesis/Platform/FileSystem.hpp"
#include "Genesis/Rendering/Camera.hpp"

#include "Genesis/Resource/PbrMesh.hpp"
#include "Genesis/Resource/PbrMaterial.hpp"

#include "Genesis/Component/Mesh.hpp"

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



	Framebuffer LegacyWorldRenderer::drawScene(World* world, Camera& camera, TransformD& camera_transform)
	{
		this->window_to_view_map.clear();

		queue<Node*> nodes;

		size_t window_index = 1;

		for (Entity* entity : world->getEntities())
		{
			nodes.push(entity->getRootNode());

			while (!nodes.empty())
			{
				Node* node = nodes.front();
				nodes.pop();

				if (node->hasComponent<WindowMeshComponent>())
				{
					WindowMeshComponent* window = node->getComponent<WindowMeshComponent>();
					if (window->view == WindowMeshComponent::WindowView::ChildWorld)
					{
						if (node->getRootEntity()->hasSubworld())
						{
							const TransformD world_origin = node->getRootEntity()->getWorldTransform();
							TransformD offset_camera_transform;
							offset_camera_transform.setPosition(glm::inverse(world_origin.getOrientation()) * (camera_transform.getPosition() - world_origin.getPosition()));
							offset_camera_transform.setOrientation(glm::inverse(world_origin.getOrientation()) * camera_transform.getOrientation());

							this->drawWorld(this->framebuffers[window_index], this->framebuffer_size, node->getRootEntity()->getSubworld(), camera, offset_camera_transform, world_origin.getScale());
							this->window_to_view_map[window] = window_index;
							window_index++;
						}
					}
				}

				for (Node* child : node->getChildren())
				{
					nodes.push(child);
				}
			}
		}

		this->drawWorld(this->framebuffers[0], this->framebuffer_size, world, camera, camera_transform);

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


	void LegacyWorldRenderer::drawWorld(Framebuffer framebuffer, vector2U framebuffer_size, World* world, Camera& camera, TransformD& camera_transform, vector3F temp_scale)
	{
		this->backend->bindFramebuffer(framebuffer);
		vector4F color = vector4F(0.0f, 0.0f, 0.0f, 1.0f);
		if (temp_scale != vector3F(1.0))
		{
			color = vector4F(1.0f, 0.0f, 0.0f, 1.0f);
		}
		this->backend->clearFramebuffer(true, true, &color);

		matrix4F scale_matrix = glm::scale(matrix4F(1.0f), temp_scale);
		matrix4F view_projection_matrix = camera.getProjectionMatrix(((float)framebuffer_size.x) / ((float)framebuffer_size.y)) * camera_transform.getViewMatirx() * scale_matrix;

		PipelineSettings settings;
		settings.cull_mode = CullMode::Back;
		settings.depth_test = DepthTest::Test_And_Write;
		settings.depth_op = DepthOp::Less;
		settings.blend_op = BlendOp::None;
		settings.src_factor = BlendFactor::One;
		settings.dst_factor = BlendFactor::Zero;
		this->backend->setPipelineState(settings);

		this->backend->bindShaderProgram(this->pbr_program);

		this->backend->setUniform3f("environment.ambient_light", vector3F(1.0f));
		this->backend->setUniform3f("environment.camera_position", (vector3F)camera_transform.getPosition());
		this->backend->setUniformMat4f("environment.view_projection_matrix", view_projection_matrix);

		for (Entity* entity : world->getEntities())
		{
			drawNodeMesh(entity->getRootNode());
		}

		this->backend->bindShaderProgram(this->window_program);

		this->backend->setUniform2f("camera_view.framebuffer_size", (vector2F)framebuffer_size);

		this->backend->setUniform3f("environment.ambient_light", vector3F(1.0f));
		this->backend->setUniform3f("environment.camera_position", (vector3F)camera_transform.getPosition());
		this->backend->setUniformMat4f("environment.view_projection_matrix", view_projection_matrix);

		for (Entity* entity : world->getEntities())
		{
			drawNodeWindow(entity->getRootNode());
		}

		this->backend->bindShaderProgram(nullptr);
		this->backend->bindFramebuffer(nullptr);
	}

	void LegacyWorldRenderer::drawNodeMesh(Node* node)
	{
		if (node->hasComponent<MeshComponent>())
		{
			PbrMesh* mesh = node->getComponent<MeshComponent>()->mesh;
			TransformD& transform = node->getGlobalTransform();

			writeTransformUniform(this->backend, transform);

			backend->bindVertexBuffer(mesh->vertex_buffer);
			backend->bindIndexBuffer(mesh->index_buffer);

			for (PbrMeshPrimitive& primitive : mesh->primitives)
			{
				PbrMaterial& material = *primitive.temp_material_ptr;
				writeMaterialUniform(this->backend, material);
				backend->drawIndex(primitive.index_count, primitive.first_index);
			}
		}

		for (Node* child : node->getChildren())
		{
			drawNodeMesh(child);
		}
	}

	void LegacyWorldRenderer::drawNodeWindow(Node* node)
	{
		if (node->hasComponent<WindowMeshComponent>())
		{
			WindowMeshComponent* window = node->getComponent<WindowMeshComponent>();
			if (has_value(this->window_to_view_map, window))
			{
				PbrMesh* mesh = node->getComponent<WindowMeshComponent>()->mesh;
				TransformD& transform = node->getGlobalTransform();

				backend->setUniformMat4f("matrices.model", transform.getModelMatrix());

				size_t window_index = this->window_to_view_map[window];
				this->backend->setUniformTexture("camera_view.framebuffer", 5, this->backend->getFramebufferColorAttachment(this->framebuffers[window_index], 0));

				backend->bindVertexBuffer(mesh->vertex_buffer);
				backend->bindIndexBuffer(mesh->index_buffer);

				for (PbrMeshPrimitive& primitive : mesh->primitives)
				{
					PbrMaterial& material = *primitive.temp_material_ptr;
					writeMaterialUniform(this->backend, material);
					backend->drawIndex(primitive.index_count, primitive.first_index);
				}
			}
		}

		for (Node* child : node->getChildren())
		{
			drawNodeWindow(child);
		}
	}
}
