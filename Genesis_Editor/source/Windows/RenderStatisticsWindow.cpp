#include "Genesis_Editor/Windows/RenderStatisticsWindow.hpp"

#include "imgui.h"

namespace Genesis
{
	RenderStatisticsWindow::RenderStatisticsWindow(LegacyBackend* backend)
	{
		this->backend = backend;
	}
	void RenderStatisticsWindow::draw(TimeStep time_step)
	{
		FrameStats stats = this->backend->getLastFrameStats();
		ImGui::Begin("Render Statistics");
		ImGui::Text("Frame Time (ms): %.2f", time_step * 1000.0);
		ImGui::Text("Draw Calls     : %u", stats.draw_calls);
		ImGui::Text("Tris count     : %u", stats.triangles_count);
		ImGui::End();
	}
}