#include "Genesis/Rendering/BaseImGui.hpp"

#include "imgui.cpp"
#include "imgui.h"
#include "imgui_demo.cpp"
#include "imgui_draw.cpp"
#include "imgui_widgets.cpp"
#include "imgui_internal.h"

using namespace Genesis;

BaseImGui::BaseImGui(InputManager* input_manager, Window* window)
{
	this->input_manager = input_manager;
	this->window = window;

	this->imgui_context = ImGui::CreateContext();
}

BaseImGui::~BaseImGui()
{
	ImGui::DestroyContext(this->imgui_context);
}

void BaseImGui::update()
{
	ImGuiIO& io = ImGui::GetIO();

	vector2F window_size = this->window->getWindowSize();

	io.DisplaySize = { window_size.x, window_size.y };

	vector2F mouse_pos = this->input_manager->getMousePosition();
	io.MousePos = ImVec2(mouse_pos.x, mouse_pos.y);

	//TODO Input
	io.MouseDown[0] = this->input_manager->getButtonDown("Mouse_Left");
	io.MouseDown[1] = this->input_manager->getButtonDown("Mouse_Right");
	io.MouseDown[2] = this->input_manager->getButtonDown("Mouse_Middle");
	io.MouseDown[3] = this->input_manager->getButtonDown("Mouse_X1");
	io.MouseDown[4] = this->input_manager->getButtonDown("Mouse_X2");

	if (this->input_manager->getButtonDown("Mouse_ForwardScroll"))
	{
		io.MouseWheel += 1;
	}
	else if (this->input_manager->getButtonDown("Mouse_BackwardScroll"))
	{
		io.MouseWheel -= 1;
	}
}
