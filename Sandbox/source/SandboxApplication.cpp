#include "Sandbox/SandboxApplication.hpp"

//Hardcode For now
#include "SDL2_Platform.hpp" 
#include "SDL2_Window.hpp" 
#include "OpenglBackend.hpp"
#include "Genesis/LegacyRendering/LegacyWorldRenderer.hpp"

#include <Genesis/Core/Types.hpp>

SandboxApplication::SandboxApplication()
{
	this->platform = new Genesis::SDL2_Platform(this);
	this->window = new Genesis::SDL2_Window(Genesis::vector2U(1600, 900), "Sandbox: ");

	this->legacy_backend = new Genesis::Opengl::OpenglBackend((Genesis::SDL2_Window*) window);
	this->world_renderer = new Genesis::LegacyWorldRenderer(this->legacy_backend);

	this->world = new Genesis::World(this->world_renderer);

	/*{
		Genesis::vector<Genesis::VertexElementType> vertex_types = { Genesis::VertexElementType::float_3, Genesis::VertexElementType::float_3 };
		Genesis::VertexInputDescriptionCreateInfo vertex_description = { vertex_types.data(), (uint32_t)vertex_types.size()};

		Genesis::vector3F vertices[] =
		{
			Genesis::vector3F(-1.0f, -1.0f, 0.0f),//Position
			Genesis::vector3F(1.0f, 0.0f, 0.0f), //Color

			Genesis::vector3F(1.0f, -1.0f, 0.0f),
			Genesis::vector3F(0.0f, 1.0f, 0.0f),

			Genesis::vector3F(0.0f, 1.0f, 0.0f),
			Genesis::vector3F(0.0f, 0.0f, 1.0f)
		};

		uint32_t indices[] = { 0, 1, 2 };

		this->vertex_buffer = this->legacy_backend->createVertexBuffer(vertices, sizeof(Genesis::vector3F) * _countof(vertices), vertex_description);
		this->index_buffer = this->legacy_backend->createIndexBuffer(indices, sizeof(uint32_t) * _countof(indices), Genesis::IndexType::uint32);
		this->index_count = _countof(indices);

		Genesis::string vertex_shader = "#version 330 core\n layout(location = 0) in vec3 position;\n layout(location = 1) in vec3 color;\n out vec3 out_color;\n void main()\n {\n out_color = color;\n gl_Position = vec4(position, 1.0);\n }\n ";
		Genesis::string fragment_shader = "#version 330 core\n in vec3 out_color;\n out vec4 FragColor;\n void main()\n {\n FragColor = vec4(out_color, 1.0);\n }\n";
		this->program = this->legacy_backend->createShaderProgram(vertex_shader.c_str(), (uint32_t)vertex_shader.size(), fragment_shader.c_str(), (uint32_t)fragment_shader.size());
	}*/
}

SandboxApplication::~SandboxApplication()
{
	delete this->world;
	delete this->world_renderer;
	delete this->legacy_backend;
}

void SandboxApplication::update(Genesis::TimeStep time_step)
{
	GENESIS_PROFILE_FUNCTION("SandboxApplication::update");
	Genesis::Application::update(time_step);

	if (this->world != nullptr)
	{
		this->world->runSimulation(this, time_step);
	}
}

void SandboxApplication::render(Genesis::TimeStep interpolation_value)
{
	GENESIS_PROFILE_FUNCTION("SandboxApplication::render");
	
	Application::render(interpolation_value);

	this->legacy_backend->startFrame();

	this->world_renderer->drawWorld(this->world);

	this->legacy_backend->endFrame();
}
