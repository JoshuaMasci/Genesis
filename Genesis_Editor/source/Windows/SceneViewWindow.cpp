#include "Genesis_Editor/Windows/SceneViewWindow.hpp"

#include "imgui.h"

namespace Genesis
{
	SceneViewWindow::SceneViewWindow(LegacyBackend* legacy_backend)
	{
		this->legacy_backend = legacy_backend;
	}

	SceneViewWindow::~SceneViewWindow()
	{
	}

	void SceneViewWindow::drawWindow(EntityWorld* world)
	{
		ImGui::Begin("Scene View");
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();
		ImVec2 window_size = ImVec2(vMax.x - vMin.x, vMax.y - vMin.y);

		ImGui::End();
	}
}