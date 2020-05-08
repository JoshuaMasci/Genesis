#pragma once

#include "Genesis/LegacyBackend/LegacyBackend.hpp"
#include "Genesis/Rendering/MeshRenderer.hpp"

#include "Genesis/LegacyRendering/LegacyMesh.hpp"
#include "Genesis/LegacyRendering/LegacyMaterial.hpp"

#include "Genesis/LegacyRendering/LegacyMeshPool.hpp"
#include "Genesis/LegacyRendering/LegacyTexturePool.hpp"
#include "Genesis/LegacyRendering/LegacyMaterialPool.hpp"

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

	protected:
		LegacyBackend* legacy_backend;

		ShaderProgram program;

		VertexBuffer vertex_buffer;
		IndexBuffer index_buffer;
		uint32_t index_count;
	};
}