#pragma once

#include "genesis_engine/input/input_manager.hpp"
#include "genesis_engine/platform/window.hpp"

struct ImGuiContext;

namespace genesis
{
	class BaseImGui
	{
	public:
		BaseImGui(InputManager* input_manager, Window* window);
		~BaseImGui();

		virtual void beginFrame() = 0;
		virtual void endFrame() = 0;

		void beginDocking();
		void endDocking();

	protected:
		void update();

		InputManager* input_manager;
		Window* window;

		ImGuiContext* imgui_context;
	};
}