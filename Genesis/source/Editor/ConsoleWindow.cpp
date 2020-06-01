#include "Genesis/Editor/ConsoleWindow.hpp"

using namespace Genesis;

ConsoleWindow::ConsoleWindow()
{
	this->name = "Console";
}

ConsoleWindow::~ConsoleWindow()
{
}

void ConsoleWindow::addMessage(ConsoleMessage& message)
{
	this->messages.push_back(message);
}

#include "imgui.h"

void ConsoleWindow::drawWindow()
{
	ImGui::Begin(this->name.c_str());

	// Options menu
	if (ImGui::BeginPopup("Options"))
	{
		ImGui::Checkbox("Auto-scroll", &this->auto_scroll);
		ImGui::EndPopup();
	}

	// Main window
	if (ImGui::Button("Options"))
	{
		ImGui::OpenPopup("Options");
	}
	ImGui::SameLine();
	bool clear = ImGui::Button("Clear");
	ImGui::SameLine();
	bool copy = ImGui::Button("Copy");

	ImGui::Separator();
	ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

	ImGuiListClipper clipper;
	clipper.Begin((int)this->messages.size());

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	while (clipper.Step())
	{
		for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
		{
			vector4F color = this->getColor(this->messages[(size_t)line_no].level);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(color.x, color.y, color.z, color.w));
			ImGui::TextUnformatted(this->messages[(size_t)line_no].message.c_str());
			ImGui::PopStyleColor();
		}
	}
	ImGui::PopStyleVar();

	if (this->auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
	{
		ImGui::SetScrollHereY(1.0f);
	}

	ImGui::EndChild();
	ImGui::End();

	if (clear)
	{
		this->messages.clear();
	}
}

vector4F ConsoleWindow::getColor(ConsoleLevel level)
{
	switch (level)
	{
	case Genesis::ConsoleLevel::trace:
		return this->colors[0];
	case Genesis::ConsoleLevel::debug:
		return this->colors[1];
	case Genesis::ConsoleLevel::info:
		return this->colors[2];
	case Genesis::ConsoleLevel::warning:
		return this->colors[3];
	case Genesis::ConsoleLevel::error:
		return this->colors[4];
	case Genesis::ConsoleLevel::critical:
		return this->colors[5];
	case Genesis::ConsoleLevel::off:
		return this->colors[6];
	}
	return this->colors[6];
}
