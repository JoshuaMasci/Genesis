#pragma once

#include "Genesis/LegacyBackend/LegacyBackend.hpp"
#include "Genesis/Rendering/Camera.hpp"

namespace Genesis
{
	class World;
	class Node;
	struct PbrMesh;

	class LegacyWorldRenderer
	{
	public:
		LegacyWorldRenderer(LegacyBackend* backend);
		~LegacyWorldRenderer();

		void drawWorld(Framebuffer framebuffer, vector2U framebuffer_size, World* world, Camera& camera, TransformD& camera_transform);

		void drawWorldWithWindow(Framebuffer framebuffer, vector2U framebuffer_size, World* world, Camera& camera, TransformD& camera_transform, PbrMesh* window_mesh, TransformD& window_trans, Texture2D window_frame);


	protected:
		LegacyBackend* backend;

		void drawNode(Node* node);

		ShaderProgram pbr_program;
		ShaderProgram window_program;
	};
}