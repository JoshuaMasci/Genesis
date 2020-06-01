#include "Genesis/LegacyRendering/LegacyRenderingSystem.hpp"

#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Frustum.hpp"

#include "Genesis/Resource/PbrMesh.hpp"
#include "Genesis/Resource/PbrMaterial.hpp"

#include "Genesis/Platform/FileSystem.hpp"

using namespace Genesis;

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
		backend->setUniformTexture("material_textures[0]", 0, material.albedo_texture);
	}

	if (material.metallic_roughness_texture != nullptr)
	{
		backend->setUniformTexture("material_textures[1]", 1, material.metallic_roughness_texture);
	}

	if (material.normal_texture != nullptr)
	{
		backend->setUniformTexture("material_textures[2]", 2, material.normal_texture);
	}

	if (material.occlusion_texture != nullptr)
	{
		backend->setUniformTexture("material_textures[3]", 3, material.occlusion_texture);
	}

	if (material.emissive_texture != nullptr)
	{
		backend->setUniformTexture("material_textures[4]", 4, material.emissive_texture);
	}
}

void writeTransformUniform(LegacyBackend* backend, TransformD& transform)
{
	backend->setUniformMat4f("matrices.model", transform.getModelMatrix());
	backend->setUniformMat3f("matrices.normal", transform.getNormalMatrix());
}

LegacyRenderingSystem::LegacyRenderingSystem(LegacyBackend* legacy_backend)
{
	this->backend = legacy_backend;

	{
		//glTF.vert
		string vert_data = "";
		string frag_data = "";

		FileSystem::loadFileString("res/shaders_opengl/glTF.vert", vert_data);
		FileSystem::loadFileString("res/shaders_opengl/glTF.frag", frag_data);
		this->shader_program = this->backend->createShaderProgram(vert_data.data(), (uint32_t)vert_data.size(), frag_data.data(), (uint32_t)frag_data.size());
	}
}

LegacyRenderingSystem::~LegacyRenderingSystem()
{
	this->backend->destoryShaderProgram(this->shader_program);
}

void LegacyRenderingSystem::render(vector2U screen_size, EcsWorld* world, TimeStep time_step)
{
	EntityHandle camera = world->main_camera;

	if (camera == entt::null)
	{
		return;
	}

	Camera& camera_component = world->entity_registry.get<Camera>(camera);
	TransformD& camera_transform = world->entity_registry.get<TransformD>(camera);
	matrix4F view_projection_matrix = camera_component.getProjectionMatrix(((float)screen_size.x) / ((float)screen_size.y)) * camera_transform.getViewMatirx();

	//Frustum frustum(view_projection_matrix);

	{
		PipelineSettings settings;
		settings.cull_mode = CullMode::Back;
		settings.depth_test = DepthTest::Test_And_Write;
		settings.depth_op = DepthOp::Less;
		settings.blend_op = BlendOp::None;
		settings.src_factor = BlendFactor::One;
		settings.dst_factor = BlendFactor::Zero;
		this->backend->setPipelineState(settings);
	}

	this->backend->bindShaderProgram(this->shader_program);

	this->backend->setUniform3f("environment.ambient_light", vector3F(1.0f));
	this->backend->setUniform3f("environment.camera_position", (vector3F)camera_transform.getPosition());
	this->backend->setUniformMat4f("environment.view_projection_matrix", view_projection_matrix);

	auto& view = world->entity_registry.view<PbrMesh, TransformD>();
	for (EntityHandle entity : view)
	{
		PbrMesh& mesh = view.get<PbrMesh>(entity);
		TransformD& transform = view.get<TransformD>(entity);

		writeTransformUniform(this->backend, transform);

		//TODO Skinning
		backend->setUniform1u("skin.joint_count", 0);

		backend->bindVertexBuffer(mesh.vertex_buffer);
		backend->bindIndexBuffer(mesh.index_buffer);

		for (PbrMeshPrimitive& primitive : mesh.primitives)
		{
			PbrMaterial& material = *primitive.temp_material_ptr;
			writeMaterialUniform(this->backend, material);
			backend->drawIndex(primitive.index_count, primitive.first_index);
		}
	}
}
