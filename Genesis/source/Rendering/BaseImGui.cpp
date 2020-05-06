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

	//Docking Enabled
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	io.KeyMap[ImGuiKey_Tab] = (int)KeyboardButton::Tab;
	io.KeyMap[ImGuiKey_LeftArrow] = (int)KeyboardButton::Left;
	io.KeyMap[ImGuiKey_RightArrow] = (int)KeyboardButton::Right;
	io.KeyMap[ImGuiKey_UpArrow] = (int)KeyboardButton::Up;
	io.KeyMap[ImGuiKey_DownArrow] = (int)KeyboardButton::Down;
	io.KeyMap[ImGuiKey_PageUp] = (int)KeyboardButton::Pageup;
	io.KeyMap[ImGuiKey_PageDown] = (int)KeyboardButton::Pagedown;
	io.KeyMap[ImGuiKey_Home] = (int)KeyboardButton::Home;
	io.KeyMap[ImGuiKey_End] = (int)KeyboardButton::End;
	io.KeyMap[ImGuiKey_Insert] = (int)KeyboardButton::Insert;
	io.KeyMap[ImGuiKey_Delete] = (int)KeyboardButton::Delete;
	io.KeyMap[ImGuiKey_Backspace] = (int)KeyboardButton::Backspace;
	io.KeyMap[ImGuiKey_Space] = (int)KeyboardButton::Space;
	io.KeyMap[ImGuiKey_Enter] = (int)KeyboardButton::Enter;
	io.KeyMap[ImGuiKey_Escape] = -1;//No escape button
	io.KeyMap[ImGuiKey_KeyPadEnter] = (int)KeyboardButton::Pad_Enter;
	io.KeyMap[ImGuiKey_A] = (int)KeyboardButton::A;
	io.KeyMap[ImGuiKey_C] = (int)KeyboardButton::C;
	io.KeyMap[ImGuiKey_V] = (int)KeyboardButton::V;
	io.KeyMap[ImGuiKey_X] = (int)KeyboardButton::X;
	io.KeyMap[ImGuiKey_Y] = (int)KeyboardButton::Y;
	io.KeyMap[ImGuiKey_Z] = (int)KeyboardButton::Z;

	//io.SetClipboardTextFn = ImGui_ImplSDL2_SetClipboardText;
	//io.GetClipboardTextFn = ImGui_ImplSDL2_GetClipboardText;
	//io.ClipboardUserData = NULL;
}

BaseImGui::~BaseImGui()
{
	ImGui::DestroyContext(this->imgui_context);
}

void BaseImGui::beginDocking()
{
	bool p_open = true;
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background 
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &p_open, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}
	else
	{
		GENESIS_ENGINE_ERROR("Imgui Docking Disabled");
	}
}

void Genesis::BaseImGui::endDocking()
{
	ImGui::End();
}

const KeyboardButton keyboard_buttons[] = { KeyboardButton::Tab, KeyboardButton::Left, KeyboardButton::Right, KeyboardButton::Up, KeyboardButton::Down, KeyboardButton::Pageup, KeyboardButton::Pagedown, KeyboardButton::Home, KeyboardButton::Insert, KeyboardButton::End, KeyboardButton::Delete, KeyboardButton::Backspace, KeyboardButton::Space, KeyboardButton::Enter, KeyboardButton::Pad_Enter, KeyboardButton::A, KeyboardButton::C, KeyboardButton::V, KeyboardButton::X, KeyboardButton::Y, KeyboardButton::Z};

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

	//Keyboard
	{
		string input_text = this->input_manager->getInputText();
		if (!input_text.empty())
		{
			io.AddInputCharactersUTF8(input_text.c_str());
		}

		for (size_t i = 0; i < _countof(keyboard_buttons); i++)
		{
			io.KeysDown[(int)keyboard_buttons[i]] = this->input_manager->getKeyboardButtonState(keyboard_buttons[i]);
		}
	}


}
