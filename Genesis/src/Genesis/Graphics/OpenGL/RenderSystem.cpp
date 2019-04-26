#include "RenderSystem.hpp"

#include "Genesis/Graphics/OpenGL/OpenGL.hpp"
#include "Genesis/Graphics/OpenGL/TexturedMesh.hpp"
#include "Genesis/Graphics/OpenGL/ShaderProgram.hpp"

#include "Genesis/WorldTransform.hpp"
#include "Genesis/Graphics/OpenGL/Camera.hpp"

using namespace Genesis;

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image.h>

GLuint LoadTexture(string file_name)
{
	int width, height, numComponents;
	unsigned char* data = stbi_load((file_name).c_str(), &width, &height, &numComponents, 4);

	if (data == NULL)
	{
		//Logger::getInstance()->logError("Can't Load the given texture: %s\n", name);
		printf("Error: Can't Load the given texture: %s\n", file_name.c_str());
		return 0;
	}

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);

	return texture;
}

void DeleteTexture(GLuint texture)
{
	glDeleteTextures(1, &texture);
}

RenderSystem::RenderSystem(Window* window)
{
	this->window = window;
	this->context = this->window->GL_CreateContext();

	// Initialize GLEW
	if (glewInit() != GLEW_OK)
	{
		printf("Failed to initialize GLEW\n");
	}

	this->window->GL_SetVsync(Vsync::ON);

	if (true)
	{
		vector<string> texture_locs =
		{
			"resources/textures/1K_Grid.png",
			"resources/textures/4K_Grid.png",
			"resources/textures/Blue.png",
			"resources/textures/Green.png",
			"resources/textures/Purple.png",
			"resources/textures/Red.png"
		};

		this->textures.resize(texture_locs.size());

		for (int i = 0; i < texture_locs.size(); i++)
		{
			this->textures[i] = LoadTexture(texture_locs[i]);
		}
	}

	if (true)
	{
		vector<string> mesh_locs =
		{
			"resources/models/cube.obj"
		};

		this->meshes.resize(mesh_locs.size());

		for (int i = 0; i < mesh_locs.size(); i++)
		{
			this->meshes[i] = TexturedMesh::loadObj(mesh_locs[i]);
		}
	}

	if (true)
	{
		vector<ShaderSet> shader_locs =
		{
			{"resources/shaders/OpenGL/Textured.vs", "resources/shaders/OpenGL/Textured.fs"},
		};

		this->shaders.resize(shader_locs.size());

		for (int i = 0; i < shader_locs.size(); i++)
		{
			this->shaders[i] = new ShaderProgram(shader_locs[i].vert, shader_locs[i].frag);
		}
	}
}

RenderSystem::~RenderSystem()
{
	for (int i = 0; i < this->textures.size(); i++)
	{
		DeleteTexture(this->textures[i]);
	}

	for (int i = 0; i < this->meshes.size(); i++)
	{
		delete this->meshes[i];
	}

	for (int i = 0; i < this->shaders.size(); i++)
	{
		delete this->shaders[i];
	}

	this->window->GL_DeleteContext(this->context);
}

matrix4F ModelMatrix(vector3D pos, quaternionD orient)
{
	return glm::translate(matrix4F(1.0F), (vector3F)(pos)) * glm::toMat4((quaternionF)orient);
}

void RenderSystem::drawFrame(EntityRegistry& registry, JobSystem& job_system, double delta_time)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Get the first camera in the scene
	matrix4F projection_matrix = matrix4F(1.0f);
	matrix4F view_matrix = matrix4F(1.0f);

	auto view1 = registry.view<Camera, WorldTransform>();
	for (auto entity : view1)
	{
		auto& camera = view1.get<Camera>(entity);
		auto& transform = view1.get<WorldTransform>(entity);
		Transform camera_transform = transform.current_transform;

		projection_matrix = camera.getProjectionMatrix(this->window->getWindowSize());
		view_matrix = glm::lookAt((vector3F)camera_transform.getPosition(), (vector3F)(camera_transform.getForward() + camera_transform.getPosition()), (vector3F)camera_transform.getUp());
		break;
	}

	auto view = registry.view<TexturedModel, WorldTransform>();
	for (auto entity : view)
	{
		auto& model = view.get<TexturedModel>(entity);
		auto& transform = view.get<WorldTransform>(entity);

		matrix4F model_matrix = ModelMatrix(transform.current_transform.getPosition(), transform.current_transform.getOrientation());

		glBindTexture(0, this->textures[model.texture]);
		this->shaders[model.shader]->setActiveProgram();
		this->shaders[model.shader]->setUniform("MVP", projection_matrix * view_matrix * model_matrix);
		this->shaders[model.shader]->setUniform("model_matrix", model_matrix);
		this->meshes[model.mesh]->draw();
	}

	this->window->GL_UpdateBuffer();
}
