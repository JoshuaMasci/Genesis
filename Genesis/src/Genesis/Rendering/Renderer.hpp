#pragma once

#include "Genesis/Entity.hpp"
#include "Genesis/Core/Transform.hpp"
#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/RenderingBackend.hpp"
#include "Genesis/Rendering/ResourceLoaders.hpp"
#include "Genesis/Rendering/ModelRenderer.hpp"
#include "Genesis/Rendering/RenderLayer.hpp"
#include "Genesis/Rendering/Lighting.hpp"
#include "Genesis/Rendering/Model.hpp"


namespace Genesis
{
	class Renderer : public RenderLayer
	{
	public:
		Renderer(RenderingBackend* backend);
		~Renderer();

		virtual void startFrame() override;
		virtual void endFrame() override;

		void drawWorld(EntityRegistry& entity_registry, EntityId camera_entity);

		vector3F ambient_light = vector3F(0.4f);

		virtual void setScreenSize(vector2U size) override;
		virtual View getView() override;
		virtual uint32_t getViewImageIndex() override;

		void tempAddMeshToList(string name, CommandBuffer* command_buffer, Mesh& mesh) { this->loaded_meshes[name] = mesh; };

	private:
		//Model Renderer
		TexturedModelRenderer textured_renderer;
		TexturedNormalModelRenderer textured_normal_renderer;

		//Resources
		map<string, Mesh> loaded_meshes;
		map<string, Texture> loaded_textures;
		map<string, Shader> loaded_shaders;

		//Main view
		View view;
		vector2U view_size;

		//Shadows
		Array<View> shadow_views;
		size_t used_shadows = 0;
		vector2U shadow_size;

		vector<View> sub_views;

		matrix4F drawDirectionalShadowView(EntityRegistry& entity_registry, View shadow_view, DirectionalLight& directional, Transform& light_transform);
		matrix4F drawSpotShadowView(EntityRegistry& entity_registry, View shadow_view, SpotLight& spot, Transform& light_transform);

	};
}