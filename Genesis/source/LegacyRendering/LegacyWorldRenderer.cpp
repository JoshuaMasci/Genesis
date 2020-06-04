#include "Genesis/LegacyRendering/LegacyWorldRenderer.hpp"

#include "Genesis/Entity/World.hpp"
#include "Genesis/Entity/Entity.hpp"

#include "Genesis/Platform/FileSystem.hpp"
#include "Genesis/Rendering/Camera.hpp"

#include "Genesis/Resource/PbrMesh.hpp"
#include "Genesis/Resource/PbrMaterial.hpp"

#include "Genesis/Entity/Mesh.hpp"

namespace Genesis
{
	LegacyWorldRenderer::LegacyWorldRenderer(LegacyBackend* backend)
	{
		this->backend = backend;

		//glTF.vert
		string vert_data = "";
		string frag_data = "";

		FileSystem::loadFileString("res/shaders_opengl/glTF.vert", vert_data);
		FileSystem::loadFileString("res/shaders_opengl/glTF.frag", frag_data);
		this->pbr_program = this->backend->createShaderProgram(vert_data.data(), (uint32_t)vert_data.size(), frag_data.data(), (uint32_t)frag_data.size());

		FileSystem::loadFileString("res/shaders_opengl/Window.vert", vert_data);
		FileSystem::loadFileString("res/shaders_opengl/Window.frag", frag_data);
		this->window_program = this->backend->createShaderProgram(vert_data.data(), (uint32_t)vert_data.size(), frag_data.data(), (uint32_t)frag_data.size());
	}

	LegacyWorldRenderer::~LegacyWorldRenderer()
	{
		this->backend->destoryShaderProgram(this->window_program);
		this->backend->destoryShaderProgram(this->pbr_program);
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
			backend->setUniformTexture("material.material_textures[0]", 0, material.albedo_texture);
		}

		if (material.metallic_roughness_texture != nullptr)
		{
			backend->setUniformTexture("material.material_textures[1]", 1, material.metallic_roughness_texture);
		}

		if (material.normal_texture != nullptr)
		{
			backend->setUniformTexture("material.material_textures[2]", 2, material.normal_texture);
		}

		if (material.occlusion_texture != nullptr)
		{
			backend->setUniformTexture("material.material_textures[3]", 3, material.occlusion_texture);
		}

		if (material.emissive_texture != nullptr)
		{
			backend->setUniformTexture("material.material_textures[4]", 4, material.emissive_texture);
		}
	}

	void writeTransformUniform(LegacyBackend* backend, TransformD& transform)
	{
		backend->setUniformMat4f("matrices.model", transform.getModelMatrix());
		backend->setUniformMat3f("matrices.normal", transform.getNormalMatrix());
	}


	void LegacyWorldRenderer::drawWorld(Framebuffer framebuffer, vector2U framebuffer_size, World* world, Camera& camera, TransformD& camera_transform)
	{
		this->backend->bindFramebuffer(framebuffer);
		this->backend->clearFramebuffer(true, true);

		/*Node* camera_node = world->getActiveCamrea();

		Camera camera = Camera(95.0f);
		TransformD camera_transform = camera_node->getGlobalTransform();*/
		matrix4F view_projection_matrix = camera.getProjectionMatrix(((float)framebuffer_size.x) / ((float)framebuffer_size.y)) * camera_transform.getViewMatirx();

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
			drawNode(entity->getRootNode());
		}

		this->backend->bindShaderProgram(nullptr);
		this->backend->bindFramebuffer(nullptr);
	}

	void LegacyWorldRenderer::drawWorldWithWindow(Framebuffer framebuffer, vector2U framebuffer_size, World* world, Camera& camera, TransformD& camera_transform, PbrMesh* window_mesh, TransformD& window_trans, Texture2D window_frame)
	{
		this->backend->bindFramebuffer(framebuffer);
		this->backend->clearFramebuffer(true, true);

		/*Node* camera_node = world->getActiveCamrea();

		Camera camera = Camera(95.0f);
		TransformD camera_transform = camera_node->getGlobalTransform();*/
		matrix4F view_projection_matrix = camera.getProjectionMatrix(((float)framebuffer_size.x) / ((float)framebuffer_size.y)) * camera_transform.getViewMatirx();

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
			drawNode(entity->getRootNode());
		}

		this->backend->bindShaderProgram(this->window_program);

		settings.cull_mode = CullMode::Back;
		settings.depth_test = DepthTest::None;
		settings.depth_op = DepthOp::Less;
		settings.blend_op = BlendOp::None;
		settings.src_factor = BlendFactor::One;
		settings.dst_factor = BlendFactor::Zero;
		this->backend->setPipelineState(settings);

		this->backend->setUniformTexture("framebuffer", 0, window_frame);
		this->backend->setUniform2f("screen_size", (vector2F)framebuffer_size);

		backend->setUniformMat4f("matrices.model", window_trans.getModelMatrix());
		backend->setUniformMat4f("matrices.view_projection_matrix", view_projection_matrix);

		writeTransformUniform(this->backend, window_trans);
		backend->bindVertexBuffer(window_mesh->vertex_buffer);
		backend->bindIndexBuffer(window_mesh->index_buffer);

		for (PbrMeshPrimitive& primitive : window_mesh->primitives)
		{
			backend->drawIndex(primitive.index_count, primitive.first_index);
		}

		this->backend->bindFramebuffer(nullptr);
	}

	void LegacyWorldRenderer::drawNode(Node* node)
	{
		if (node->getName() == "Window")
		{
			return;
		}

		if (node->hasComponent<MeshComponent>())
		{
			PbrMesh* mesh = node->getComponent<MeshComponent>()->mesh;
			TransformD& transform = node->getGlobalTransform();

			writeTransformUniform(this->backend, transform);

			//TODO Skinning
			backend->setUniform1u("skin.joint_count", 0);

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
			drawNode(child);
		}
	}
}
