#pragma once

#include "Genesis/Rendering/BaseWorldRenderer.hpp"
#include "Genesis/LegacyBackend/LegacyBackend.hpp"

#include "Genesis/LegacyRendering/LegacyMesh.hpp"
#include "Genesis/LegacyRendering/LegacyMaterial.hpp"

#include "Genesis/LegacyRendering/LegacyMeshPool.hpp"
#include "Genesis/LegacyRendering/LegacyTexturePool.hpp"
#include "Genesis/LegacyRendering/LegacyMaterialPool.hpp"

#include "Genesis/Entity/World.hpp"
#include "Genesis/Rendering/Frustum.hpp"

namespace Genesis
{
	struct EnvironmentData
	{
		vector3F camera_position;
		vector3F ambient_light;
		matrix4F view_projection_matrix;
	};

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

		virtual void addMesh(EntityRegistry* registry, EntityHandle entity, const string& mesh_file, const string& material_file) override;
		virtual void removeEntity(EntityRegistry* registry, EntityHandle entity) override;
		virtual void drawWorld(World* world, vector2U size) override;

	protected: 
		void drawAmbient(EntityRegistry* entity_registry, EnvironmentData* environment, Frustum* frustum);
		void drawDirectional(EntityRegistry* entity_registry, EnvironmentData* environment, Frustum* frustum);
		void drawPoint(EntityRegistry* entity_registry, EnvironmentData* environment, Frustum* frustum);
		void drawSpot(EntityRegistry* entity_registry, EnvironmentData* environment, Frustum* frustum);

		LegacyBackend* legacy_backend;

		LegacyMeshPool* mesh_pool;
		LegacyTexturePool* texture_pool;
		LegacyMaterialPool* material_pool;

		//TEMP
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