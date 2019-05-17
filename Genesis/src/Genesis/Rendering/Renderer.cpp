#include "Renderer.hpp"

using namespace Genesis;

Renderer::Renderer(RenderingBackend* backend)
{
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
	this->cube_vertices = backend->createBuffer(sizeof(Vertex) * vertices.size(), BufferType::Vertex, MemoryUsage::CPU_Visable);
	this->cube_vertices->fillBuffer(vertices.data(), sizeof(Vertex) * vertices.size());

	std::vector<uint16_t> indices =
	{
	5, 6, 4, 4, 6, 7, //front
	1, 0, 2, 2, 0, 3, //back
	1, 2, 5, 5, 2, 6, //left
	0, 4, 3, 3, 4, 7, //right
	2, 3, 6, 6, 3, 7, //top
	0, 1, 4, 4, 1, 5 //bottom
	};
	this->cube_indices = backend->createBuffer(sizeof(uint16_t) * indices.size(), BufferType::Index, MemoryUsage::CPU_Visable);
	this->cube_indices->fillBuffer(indices.data(), sizeof(uint16_t) * indices.size());
}

Renderer::~Renderer()
{
	delete this->cube_vertices;
	delete this->cube_indices;
}

void Renderer::drawFrame()
{
}
