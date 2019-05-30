#include "Renderer.hpp"

#include "Genesis/Application.hpp"

#include <random>

using namespace Genesis;

Renderer::Renderer(RenderingBackend* backend)
{
	this->backend = backend;

	//Cube
	std::vector<Vertex> vertices =
	{
	{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
	{{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
	{{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
	{{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
	{{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
	{{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}}
	};
	this->cube_vertices = this->backend->createBuffer(sizeof(Vertex) * vertices.size(), BufferType::Vertex, MemoryUsage::GPU_Only);
	this->cube_vertices->fill(vertices.data(), sizeof(Vertex) * vertices.size());

	std::vector<uint16_t> indices =
	{
	5, 6, 4, 4, 6, 7, //front
	1, 0, 2, 2, 0, 3, //back
	1, 2, 5, 5, 2, 6, //left
	0, 4, 3, 3, 4, 7, //right
	2, 3, 6, 6, 3, 7, //top
	0, 1, 4, 4, 1, 5 //bottom
	};
	this->cube_indices = this->backend->createBuffer(sizeof(uint16_t) * indices.size(), BufferType::Index, MemoryUsage::GPU_Only);
	this->cube_indices->fill(indices.data(), sizeof(uint16_t) * indices.size());
	this->cube_indices_count = (uint32_t)indices.size();


	//TEMP
	std::random_device random;
	std::mt19937 e2(random());
	std::uniform_real_distribution<float> rand_dist(-2.0, 2.0);
	const uint32_t threads = 12;
	this->positions.resize(threads);

	for (int i = 0; i < this->positions.size(); i++)
	{
		this->positions[i].x = rand_dist(e2);
		this->positions[i].y = rand_dist(e2);
		this->positions[i].z = rand_dist(e2);
	}
}

Renderer::~Renderer()
{
	delete this->cube_vertices;
	delete this->cube_indices;
}

void Renderer::drawFrame()
{
	float fov = 1.0f / tan(glm::radians(75.0f) / 2.0f);
	float aspect = 2560.0f / 1440.0f;
	matrix4F view = glm::lookAt(vector3F(0.0f, 0.5f, -8.0f), vector3F(0.0f), vector3F(0.0f, 1.0f, 0.0f));
	matrix4F proj = glm::infinitePerspective(fov, aspect, 0.1f);
	proj[1][1] *= -1; //Need to apply this because vulkan flips the y-axis and that's not what I need

	matrix4F mv = proj * view;

	bool result = this->backend->beginFrame();

	if (result)
	{
		for (int i = 0; i < this->positions.size(); i++)
		{
			matrix4F mvp = mv * glm::translate(matrix4F(1.0), this->positions[i]);
			this->backend->drawMeshScreen(i % 12, this->cube_vertices, this->cube_indices, this->cube_indices_count, mvp);
		}

		this->backend->endFrame();
	}
}
