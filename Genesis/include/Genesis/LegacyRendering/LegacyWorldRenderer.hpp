#pragma once

#include "Genesis/Rendering/BaseWorldRenderer.hpp"
#include "Genesis/LegacyBackend/LegacyBackend.hpp"

#include "Genesis/LegacyRendering/LegacyMesh.hpp"
#include "Genesis/LegacyRendering/LegacyMaterial.hpp"

#include "Genesis/LegacyRendering/LegacyMeshPool.hpp"
#include "Genesis/LegacyRendering/LegacyTexturePool.hpp"
#include "Genesis/LegacyRendering/LegacyMaterialPool.hpp"

#include "Genesis/Entity/World.hpp"

namespace Genesis
{
	struct LegacyMeshComponent
	{
		LegacyMeshComponent(LegacyMesh* mesh, LegacyMaterial* material) { this->mesh = mesh; this->material = material; };
		LegacyMesh* mesh;
		LegacyMaterial* material;
	};

	class LegacyWorldRenderer : public BaseWorldRenderer
	{
	public:
		LegacyWorldRenderer(LegacyBackend* backend);
		~LegacyWorldRenderer();

		virtual void addEntity(EntityRegistry* registry, EntityHandle entity, const string& mesh_file, const string& material_file) override;
		virtual void removeEntity(EntityRegistry* registry, EntityHandle entity) override;
		virtual void drawWorld(World* world) override;

	protected:
		LegacyBackend* legacy_backend;

		LegacyMeshPool* mesh_pool;
		LegacyTexturePool* texture_pool;
		LegacyMaterialPool* material_pool;

		//TEMP
		ShaderProgram color_ambient;
		ShaderProgram texture_ambient;

		ShaderProgram color_directional;
	};
}