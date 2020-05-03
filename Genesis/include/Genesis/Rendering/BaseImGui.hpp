#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Input/InputManager.hpp"
#include "Genesis/Platform/Window.hpp"

struct ImGuiContext;

namespace Genesis
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