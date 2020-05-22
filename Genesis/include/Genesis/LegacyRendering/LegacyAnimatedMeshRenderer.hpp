#pragma once

#include "Genesis/LegacyBackend/LegacyBackend.hpp"
#include "Genesis/Rendering/MeshRenderer.hpp"

#include "Genesis/Resource/GltfLoader.hpp"

namespace Genesis
{
	class LegacyAnimatedMeshRenderer : public MeshRenderer
	{
	public:
		LegacyAnimatedMeshRenderer(LegacyBackend* backend);
		~LegacyAnimatedMeshRenderer();

		virtual void drawAmbientPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum) override;

		virtual void drawDirectionalPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, DirectionalLight& light, TransformD& light_transform) override;
		virtual void drawPointPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, PointLight& light, TransformD& light_transform) override;
		virtual void drawSpotPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, SpotLight& light, TransformD& light_transform) override;

		virtual void drawAmbientPass(World* world, SceneData* environment, Frustum* frustum) override;

	protected:
		LegacyBackend* legacy_backend;

		ShaderProgram program;

		GltfModel* model;
	};
}