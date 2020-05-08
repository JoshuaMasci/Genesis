#include "Genesis/LegacyRendering/LegacyWorldRenderer.hpp"

#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Lights.hpp"

#include "Genesis/LegacyRendering/LegacyMeshRenderer.hpp"
#include "Genesis/LegacyRendering/LegacyAnimatedMeshRenderer.hpp"

using namespace Genesis;

LegacyWorldRenderer::LegacyWorldRenderer(LegacyBackend* backend)
{
	this->legacy_backend = backend;

	this->renderers.push_back(new LegacyMeshRenderer(this->legacy_backend));
}

LegacyWorldRenderer::~LegacyWorldRenderer()
{
	for (MeshRenderer* renderer : this->renderers)
	{
		delete renderer;
	}
}

void LegacyWorldRenderer::drawWorld(World* world, vector2U size)
{
	float aspect_ratio = ((float)size.x) / ((float)size.y);

	EntityHandle camera = world->getCamera();
	Camera& camera_component = world->getEntityRegistry()->get<Camera>(camera);
	TransformD& camera_transform = world->getEntityRegistry()->get<TransformD>(camera);
	matrix4F view_projection_matrix = camera_component.getProjectionMatrix(aspect_ratio) * camera_transform.getViewMatirx();

	Frustum frustum(view_projection_matrix);
	SceneData environment = { (vector3F)camera_transform.getPosition(), vector3F(0.1f), view_projection_matrix };

	PipelineSettings settings;
	settings.cull_mode = CullMode::Back;
	settings.depth_test = DepthTest::Test_And_Write;
	settings.depth_op = DepthOp::Less;
	settings.blend_op = BlendOp::None;
	settings.src_factor = BlendFactor::One;
	settings.dst_factor = BlendFactor::Zero;
	this->legacy_backend->setPipelineState(settings);
	for (MeshRenderer* renderer : this->renderers)
	{
		renderer->drawAmbientPass(world->getEntityRegistry(), &environment, &frustum);

		SceneData temp = { (vector3F)camera_transform.getPosition(), vector3F(1.0f), view_projection_matrix };
		renderer->drawAmbientPass(world, &temp, &frustum);
	}

	PipelineSettings light_settings;
	light_settings.cull_mode = CullMode::Back;
	light_settings.depth_test = DepthTest::Test_Only;
	light_settings.depth_op = DepthOp::Equal;
	light_settings.blend_op = BlendOp::Add;
	light_settings.src_factor = BlendFactor::One;
	light_settings.dst_factor = BlendFactor::One;
	this->legacy_backend->setPipelineState(light_settings);

	auto directional_light_view = world->getEntityRegistry()->view<DirectionalLight, TransformD>();
	for (EntityHandle light_entity : directional_light_view)
	{
		DirectionalLight& light = directional_light_view.get<DirectionalLight>(light_entity);
		TransformD& light_transform = directional_light_view.get<TransformD>(light_entity);

		for (MeshRenderer* renderer : this->renderers)
		{
			renderer->drawDirectionalPass(world->getEntityRegistry(), &environment, &frustum, light, light_transform);
		}
	}

	auto point_light_view = world->getEntityRegistry()->view<PointLight, TransformD>();
	for (EntityHandle light_entity : point_light_view)
	{
		PointLight& light = point_light_view.get<PointLight>(light_entity);
		TransformD& light_transform = directional_light_view.get<TransformD>(light_entity);

		for (MeshRenderer* renderer : this->renderers)
		{
			renderer->drawPointPass(world->getEntityRegistry(), &environment, &frustum, light, light_transform);
		}
	}

	auto spot_light_view = world->getEntityRegistry()->view<SpotLight, TransformD>();
	for (EntityHandle light_entity : spot_light_view)
	{
		SpotLight& light = spot_light_view.get<SpotLight>(light_entity);
		TransformD& light_transform = directional_light_view.get<TransformD>(light_entity);

		for (MeshRenderer* renderer : this->renderers)
		{
			renderer->drawSpotPass(world->getEntityRegistry(), &environment, &frustum, light, light_transform);
		}
	}
}

