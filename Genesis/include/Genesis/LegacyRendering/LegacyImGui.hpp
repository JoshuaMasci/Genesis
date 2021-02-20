#pragma once

#include "Genesis/LegacyBackend/LegacyBackend.hpp"
#include "Genesis/Rendering/BaseImGui.hpp"

namespace Genesis
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