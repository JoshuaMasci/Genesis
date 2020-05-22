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
	struct LegacyMeshComponent
	{
		LegacyMeshComponent(string mesh) { this->mesh_file = mesh; this->mesh = nullptr;};
		string mesh_file;
		LegacyMesh* mesh;
	};

	struct LegacyMaterialComponent
	{
		LegacyMaterialComponent(string material) { this->material_file = material; this->material = nullptr; };
		string material_file;
		LegacyMaterial* material;
	};

	class LegacyMeshRenderer : public MeshRenderer
	{
	public:
		LegacyMeshRenderer(LegacyBackend* backend);
		~LegacyMeshRenderer();

		virtual void drawAmbientPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum) override;
		virtual void drawAmbientPass(World* world, SceneData* environment, Frustum* frustum) override;

		virtual void drawDirectionalPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, DirectionalLight& light, TransformD& light_transform) override;
		virtual void drawPointPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, PointLight& light, TransformD& light_transform) override;
		virtual void drawSpotPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, SpotLight& light, TransformD& light_transform) override;

	protected:
		//TEMP
		void drawEntity(Entity* entity);

		LegacyBackend* legacy_backend;

		LegacyMeshPool* mesh_pool;
		LegacyTexturePool* texture_pool;
		LegacyMaterialPool* material_pool;

		ShaderProgram color_ambient;
		ShaderProgram texture_ambient;

		ShaderProgram color_directional;
		ShaderProgram texture_directional;

		ShaderProgram color_point;
		ShaderProgram texture_point;

		ShaderProgram color_spot;
		ShaderProgram texture_spot;
	};
}