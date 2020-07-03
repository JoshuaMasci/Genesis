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

		Framebuffer drawScene(Camera& camera, TransformD& camera_transform);

	protected:
		LegacyBackend* backend;

		ShaderProgram pbr_program;
		ShaderProgram window_program;
		
		const uint8_t framebuffer_count = 8;
		vector2U framebuffer_size;
		vector<Framebuffer> framebuffers;
	};
}