#pragma once

#include "Genesis/LegacyBackend/LegacyBackend.hpp"
#include "Genesis/Rendering/Camera.hpp"

namespace Genesis
{
	class World;
	class Node;
	struct PbrMesh;
	class WindowMeshComponent;

	class LegacyWorldRenderer
	{
	public:
		LegacyWorldRenderer(LegacyBackend* backend, vector2U size);
		~LegacyWorldRenderer();

		void rebuildFramebuffers(vector2U size);

		Framebuffer drawScene(World* world, Camera& camera, TransformD& camera_transform, uint8_t framebuffer_return);

		void drawWorld(Framebuffer framebuffer, vector2U framebuffer_size, World* world, Camera& camera, TransformD& camera_transform, vector3F temp_scale = vector3F(1.0f));

	protected:
		map<WindowMeshComponent*, size_t> window_to_view_map;

		LegacyBackend* backend;

		void drawNodeMesh(Node* node);
		void drawNodeWindow(Node* node);

		ShaderProgram pbr_program;
		ShaderProgram window_program;
		
		const uint8_t framebuffer_count = 8;
		vector2U framebuffer_size;
		vector<Framebuffer> framebuffers;
	};
}