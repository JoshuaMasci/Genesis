#include "RenderSystem.hpp"

#include "Genesis/Graphics/OpenGL/OpenGL.hpp"
#include "Genesis/Graphics/OpenGL/TexturedMesh.hpp"

using namespace Genesis;

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image.h>

GLuint LoadTexture(string filename)
{
	int width, height, numComponents;
	unsigned char* data = stbi_load((filename).c_str(), &width, &height, &numComponents, 4);

	if (data == NULL)
	{
		//Logger::getInstance()->logError("Can't Load the given texture: %s\n", name);
		printf("Can't Load the given texture: %s\n", filename.c_str());
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

	this->window->GL_SetVsync(Vsync::ON);

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

RenderSystem::~RenderSystem()
{
	this->window->GL_DeleteContext(this->context);

	for (int i = 0; i < this->textures.size(); i++)
	{
		DeleteTexture(this->textures[i]);
	}
}

void RenderSystem::drawFrame(EntityRegistry& registry, JobSystem& job_system, double delta_time)
{
	glClearColor(0.2f, 0.0f, 0.2f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	this->window->GL_UpdateBuffer();
}
