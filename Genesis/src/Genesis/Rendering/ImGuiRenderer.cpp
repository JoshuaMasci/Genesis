#include "ImGuiRenderer.hpp" 

using namespace Genesis;

#include "imgui.cpp"
#include "imgui.h"
#include "imgui_demo.cpp"
#include "imgui_draw.cpp"
#include "imgui_widgets.cpp"
#include "imgui_internal.h"

ImGuiRenderer::ImGuiRenderer(RenderingBackend* backend)
{
	this->backend = backend;
	
	Array<ImageFormat> color(1);
	color[0] = ImageFormat::RGBA_8_UNorm;
	this->layout = FramebufferLayout(color, ImageFormat::Invalid);

	this->view = this->backend->createView(vector2U(1024), this->layout);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	int width, height;
	unsigned char* pixels = NULL;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
	this->fonts = this->backend->createTexture(vector2U(width, height), pixels, (width * height) * 4);
	io.Fonts->TexID = this->fonts;
}

ImGuiRenderer::~ImGuiRenderer()
{
	this->backend->destroyView(this->view);
	this->backend->destroyTexture(this->fonts);
}

void ImGuiRenderer::startFrame()
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize.x = 1920.0f;
	io.DisplaySize.y = 1080.0f;

	ImGui::NewFrame();

	bool show_demo = true;
	ImGui::ShowDemoWindow(&show_demo);
}

void ImGuiRenderer::endFrame()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImDrawData* draw_data = ImGui::GetDrawData();

	printf("");
}
