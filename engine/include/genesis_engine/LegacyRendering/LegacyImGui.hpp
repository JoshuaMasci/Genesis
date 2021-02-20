#pragma once

#include "genesis_engine/LegacyBackend/LegacyBackend.hpp"
#include "genesis_engine/rendering/BaseImGui.hpp"

namespace genesis_engine
{
	class LegacyImGui : public BaseImGui
	{
	public:
		LegacyImGui(LegacyBackend* backend, InputManager* input_manager, Window* window);
		~LegacyImGui();

		virtual void beginFrame() override;
		virtual void endFrame() override;

	private:
		LegacyBackend* backend;

		ShaderProgram imgui_program;
		Texture2D texture_atlas;

		vector<VertexElementType> vertex_elements;
		VertexInputDescriptionCreateInfo vertex_create_info;

		PipelineSettings settings;

		VertexBuffer temp_vertex;
		IndexBuffer temp_index;
	};
}