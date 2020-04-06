#include "Genesis/Entity/WorldRenderer.hpp"

#include "Genesis/Debug/Assert.hpp"
#include "Genesis/Debug/Profiler.hpp"

#include "Genesis/Core/Transform.hpp"
#include "Genesis/Rendering/Frustum.hpp"
#include "Genesis/Rendering/ResourceLoaders.hpp"
#include "Genesis/Entity/MeshComponent.hpp"
#include "Genesis/Rendering/Camera.hpp"

using namespace Genesis;

WorldRenderer::WorldRenderer(RenderingBackend* backend)
{
}

WorldRenderer::~WorldRenderer()
{
}

void WorldRenderer::drawWorld(World* world)
{
	GENESIS_PROFILE_FUNCTION("WorldRenderer::drawWorld");

	/*this->draw_call_count = 0;

	vector2U temp_size = this->backend->getScreenSize();
	if (temp_size != this->view_size)
	{
		this->view_size = temp_size;
		this->backend->resizeFramebuffer(this->framebuffer, this->view_size);
	}

	CommandBufferInterface* command_buffer = this->backend->beginSTCommandBuffer(this->st_command_buffer, this->framebuffer);

	{
		float aspect_ratio = ((float)this->view_size.x) / ((float)this->view_size.y);

		EntityHandle camera = world->getCamera();
		Camera& camera_component = world->getEntityRegistry()->get<Camera>(camera);
		TransformF camera_transform = world->getEntityRegistry()->get<TransformD>(camera).toTransformF();
		matrix4F view_projection_matrix = camera_component.getProjectionMatrix(aspect_ratio) * camera_transform.getViewMatirx();

		{
			//Update Scene Uniform
			SceneUniform temp_uniform;
			temp_uniform.camera_position = camera_transform.getPosition();
			temp_uniform.ambient_light = vector3F(1.0f);
			temp_uniform.view_projection_matrix = view_projection_matrix;

			if ((this->scene_uniform.camera_position != temp_uniform.camera_position) || (this->scene_uniform.ambient_light != temp_uniform.ambient_light) || (this->scene_uniform.view_projection_matrix != temp_uniform.view_projection_matrix))
			{
				this->scene_uniform = temp_uniform;
				this->backend->writeDynamicBuffer(this->scene_uniform_buffer, &this->scene_uniform, sizeof(SceneUniform));
			}
		}

		PipelineSettings pipeline_settings;

		{
			command_buffer->setPipelineSettings(pipeline_settings);
			command_buffer->setShader(this->mesh_shader);

			command_buffer->setUniformDynamicBuffer(0, 0, this->scene_uniform_buffer);			
		}

		Frustum frustum(view_projection_matrix);

		auto& view = world->getEntityRegistry()->view<MeshComponent, TransformD>();
		Material* material = nullptr;
		for (EntityHandle entity : view)
		{
			MeshComponent& mesh_component = view.get<MeshComponent>(entity);
			TransformF render_transform = view.get<TransformD>(entity).toTransformF();

			if (frustum.sphereTest(render_transform.getPosition(), mesh_component.mesh->frustum_sphere_radius))
			{
				command_buffer->setDescriptorSet(1, mesh_component.material->material_descriptor);

				ObjectTransformUniform matrices = {};
				matrices.model_matrix = render_transform.getModelMatrix();
				matrices.normal_matrix = (glm::mat3x4)render_transform.getNormalMatrix();

				command_buffer->setUniformConstant(&matrices, sizeof(ObjectTransformUniform));

				command_buffer->setVertexBuffer(mesh_component.mesh->vertex_buffer, *mesh_component.mesh->vertex_description);
				command_buffer->setIndexBuffer(mesh_component.mesh->index_buffer, mesh_component.mesh->index_type);

				this->draw_call_count++;
				command_buffer->drawIndexed(mesh_component.mesh->index_count);
			}
		}
	}

	this->backend->endSTCommandBuffer(this->st_command_buffer);*/
}
