#include "Genesis/LegacyRendering/LegacyAnimatedMeshRenderer.hpp"

#include "Genesis/Debug/Log.hpp"

#include "Genesis/Resource/GltfLoader.hpp"

using namespace Genesis;

LegacyAnimatedMeshRenderer::LegacyAnimatedMeshRenderer(LegacyBackend* backend)
{
	this->legacy_backend = backend;

	string file_name = "res/CesiumMan.gltf";

	tinygltf::Model gltfModel;
	tinygltf::TinyGLTF loader;
	string error;
	string warning;

	bool ret = loader.LoadASCIIFromFile(&gltfModel, &error, &warning, file_name);

	if (!error.empty())
	{
		GENESIS_ENGINE_ERROR("Error: {}", error);
	}

	if (!warning.empty())
	{
		GENESIS_ENGINE_WARNING("Warning: {}", warning);
	}

	if (!ret)
	{
		GENESIS_ENGINE_CRITICAL("Failed to parse glTF");
	}

	GltfModel* model = new GltfModel(this->legacy_backend, gltfModel);
	delete model;
}

LegacyAnimatedMeshRenderer::~LegacyAnimatedMeshRenderer()
{

}

void LegacyAnimatedMeshRenderer::drawAmbientPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum)
{
}

void LegacyAnimatedMeshRenderer::drawDirectionalPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, DirectionalLight& light, TransformF& light_transform)
{
}

void LegacyAnimatedMeshRenderer::drawPointPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, PointLight& light, TransformF& light_transform)
{
}

void LegacyAnimatedMeshRenderer::drawSpotPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, SpotLight& light, TransformF& light_transform)
{
}
