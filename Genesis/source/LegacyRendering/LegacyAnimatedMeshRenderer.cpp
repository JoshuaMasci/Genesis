#include "Genesis/LegacyRendering/LegacyAnimatedMeshRenderer.hpp"

#include "Genesis/Debug/Log.hpp"

#include "Genesis/Platform/FileSystem.hpp"

using namespace Genesis;

LegacyAnimatedMeshRenderer::LegacyAnimatedMeshRenderer(LegacyBackend* backend)
{
	this->legacy_backend = backend;

	string file_name = "res/CesiumMan.gltf";

	tinygltf::Model gltfModel;
	tinygltf::TinyGLTF loader;
	string error;
	string warning;
	bool return_value;

	if (file_name.substr(file_name.find_last_of(".") + 1) == "gltf")
	{
		return_value = loader.LoadASCIIFromFile(&gltfModel, &error, &warning, file_name);
	}
	else if (file_name.substr(file_name.find_last_of(".") + 1) == "glb")
	{
		return_value = loader.LoadBinaryFromFile(&gltfModel, &error, &warning, file_name);
	}
	else
	{
		GENESIS_ENGINE_ERROR("Unknown File extension");
	}

	if (!error.empty())
	{
		GENESIS_ENGINE_ERROR("Error: {}", error);
	}

	if (!warning.empty())
	{
		GENESIS_ENGINE_WARNING("Warning: {}", warning);
	}

	if (!return_value)
	{
		GENESIS_ENGINE_CRITICAL("Failed to parse glTF");
	}

	model = new GltfModel(this->legacy_backend, gltfModel);

	{
		//glTF.vert
		string vert_data = "";
		string frag_data = "";

		FileSystem::loadFileString("res/shaders_opengl/glTF.vert", vert_data);
		FileSystem::loadFileString("res/shaders_opengl/glTF.frag", frag_data);
		this->program = this->legacy_backend->createShaderProgram(vert_data.data(), (uint32_t)vert_data.size(), frag_data.data(), (uint32_t)frag_data.size());
	}
}

LegacyAnimatedMeshRenderer::~LegacyAnimatedMeshRenderer()
{
	delete model;
	this->legacy_backend->destoryShaderProgram(program);
}

void LegacyAnimatedMeshRenderer::drawAmbientPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum)
{
}

void LegacyAnimatedMeshRenderer::drawDirectionalPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, DirectionalLight& light, TransformD& light_transform)
{
}

void LegacyAnimatedMeshRenderer::drawPointPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, PointLight& light, TransformD& light_transform)
{
}

void LegacyAnimatedMeshRenderer::drawSpotPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, SpotLight& light, TransformD& light_transform)
{
}

void drawNode(LegacyBackend* backend, GltfNode* node, const TransformF& global_transform)
{
	TransformF& render_transform = node->local_transform.transformBy(global_transform);

	if (node->mesh != nullptr)
	{
		GltfMesh* mesh = node->mesh;

		//Joint Info
		if (node->skin != nullptr)
		{
			backend->setUniform1u("skin.joint_count", node->skin->joint_matrices.size());

			for (uint32_t i = 0; i < node->skin->joint_matrices.size(); i++)
			{
				backend->setUniformMat4f("skin.joint_matrices[" + std::to_string(i) + "]", node->skin->joint_matrices[i]);
			}
		}
		else
		{
			backend->setUniform1u("skin.joint_count", 0);
		}

		//Matrices
		backend->setUniformMat4f("matrices.model", render_transform.getModelMatrix());
		backend->setUniformMat3f("matrices.normal", render_transform.getNormalMatrix());

		backend->bindVertexBuffer(mesh->vertices);
		backend->bindIndexBuffer(mesh->indices);

		for (GltfMeshPrimitive& primitive : mesh->primitives)
		{
			GltfMaterial& material = *primitive.material;

			backend->setUniform4f("material.albedo", material.albedo_factor);
			backend->setUniform2f("material.metallic_roughness", material.metallic_roughness_factor);
			backend->setUniform4f("material.emissive", material.emissive_factor);

			backend->setUniform1i("material.albedo_texture_set", material.texture_sets.albedo);
			backend->setUniform1i("material.metallic_roughness_texture_set", material.texture_sets.metallic_roughness);
			backend->setUniform1i("material.normal_texture_set", material.texture_sets.normal);
			backend->setUniform1i("material.occlusion_texture_set", material.texture_sets.occlusion);
			backend->setUniform1i("material.emissive_texture_set", material.texture_sets.emissive);

			backend->setUniformTexture("material_textures[0]", 0, material.albedo_texture);
			backend->setUniformTexture("material_textures[1]", 1, material.metallic_roughness_texture);
			backend->setUniformTexture("material_textures[2]", 2, material.normal_texture);
			backend->setUniformTexture("material_textures[3]", 3, material.occlusion_texture);
			backend->setUniformTexture("material_textures[4]", 4, material.emissive_texture);

			backend->drawIndex(primitive.index_count, primitive.first_index);
		}
	}

	for (GltfNode* child : node->child_nodes)
	{
		drawNode(backend, child, render_transform);
	}
}

void LegacyAnimatedMeshRenderer::drawAmbientPass(World* world, SceneData* environment, Frustum* frustum)
{
	const float timestep = 1.0f / 60.0f;
	static float time = 0.0;

	if (time > model->getAnimationLength(0))
	{
		time -= model->getAnimationLength(0);
	}


	model->updateSkins();

	this->legacy_backend->bindShaderProgram(this->program);

	this->legacy_backend->setUniform3f("environment.camera_position", environment->camera_position);
	this->legacy_backend->setUniform3f("environment.ambient_light", environment->ambient_light);
	this->legacy_backend->setUniformMat4f("environment.view_projection_matrix", environment->view_projection_matrix);

	TransformF transform;

	for (GltfNode* root : this->model->root_nodes)
	{
		drawNode(this->legacy_backend, root, transform);
	}

	this->legacy_backend->bindShaderProgram(nullptr);

	model->playAnimation(0, time);
	time += timestep;
}
