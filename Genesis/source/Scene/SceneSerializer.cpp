#include "Genesis/Scene/SceneSerializer.hpp"

#include <fstream>
#include "Genesis/Core/Yaml.hpp"

#include "Genesis/Scene/Entity.hpp"

#include "Genesis/Component/NameComponent.hpp"
#include "Genesis/Component/TransformComponent.hpp"
#include "Genesis/Component/ModelComponent.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Lights.hpp"
#include "Genesis/Component/PhysicsComponents.hpp"

#include "Genesis/Resource/ResourceManager.hpp"

namespace Genesis
{
	YAML::Node serializeEntity(Entity entity)
	{
		YAML::Node entity_node;
		entity_node["Entity_UUID"] = 0;//TODO

		if (entity.has<NameComponent>())
		{
			entity_node["Name"] = entity.get<NameComponent>().data;
		}

		if (entity.has<Transform>())
		{
			Transform& transform = entity.get<Transform>();
			YAML::Node transform_node;
			transform_node["Position"] = transform.getPosition();
			transform_node["Orientation"] = transform.getOrientation();
			transform_node["Scale"] = transform.getScale();
			entity_node["Transform"] = transform_node;
		}

		if (entity.has<ModelComponent>())
		{
			ModelComponent& model = entity.get<ModelComponent>();
			YAML::Node model_node;
			model_node["Mesh"] = model.mesh->getName();
			model_node["Material"] = model.material->getName();
			entity_node["Model"] = model_node;
		}

		
		if (entity.has<DirectionalLight>())
		{
			DirectionalLight& light = entity.get<DirectionalLight>();
			YAML::Node light_node;
			light_node["enabled"] = light.enabled;
			light_node["color"] = light.color;
			light_node["intensity"] = light.intensity;
			entity_node["DirectionalLight"] = light_node;
		}

		if (entity.has<PointLight>())
		{
			PointLight& light = entity.get<PointLight>();
			YAML::Node light_node;
			light_node["enabled"] = light.enabled;
			light_node["color"] = light.color;
			light_node["intensity"] = light.intensity;
			light_node["range"] = light.range;
			light_node["attenuation"] = light.attenuation;
			entity_node["PointLight"] = light_node;
		}

		if (entity.has<RigidBodyTemplate>())
		{
			RigidBodyTemplate& rigid_body = entity.get<RigidBodyTemplate>();
			YAML::Node body_node;
			body_node["type"] = (int)rigid_body.type;
			body_node["mass"] = rigid_body.mass;
			body_node["gravity_enabled"] = rigid_body.gravity_enabled;
			body_node["is_allowed_to_sleep"] = rigid_body.is_allowed_to_sleep;
			body_node["linear_velocity"] = rigid_body.linear_velocity;
			body_node["angular_velocity"] = rigid_body.angular_velocity;
			entity_node["RigidBody"] = body_node;
		}

		if (entity.has<CollisionShapeTemplate>())
		{
			CollisionShapeTemplate& collision_shape = entity.get<CollisionShapeTemplate>();
			YAML::Node shape_node;
			shape_node["type"] = (int)collision_shape.type;
			switch (collision_shape.type)
			{
			case ShapeType::Box:
				shape_node["data"] = collision_shape.type_data.box_size;
				break;
			case ShapeType::Sphere:
				shape_node["data"] = collision_shape.type_data.sphere_radius;
				break;
			case ShapeType::Capsule:
				shape_node["data"] = collision_shape.type_data.capsule_size;
				break;
			}

			entity_node["CollisionShape"] = shape_node;
		}

		for (auto child_handle : entity.children())
		{
			entity_node["Children"].push_back(serializeEntity(Entity(entity.get_scene(), child_handle)));
		}

		return entity_node;
	}

	Entity deserializeEntity(YAML::Node& entity_node, Scene* scene, ResourceManager* resource_manager)
	{
		string entity_name = "Yaml Entity";

		if (entity_node["Name"])
		{
			entity_name = entity_node["Name"].as<std::string>();
		}

		Entity entity = scene->createEntity(entity_name.c_str());

		if (entity_node["Transform"])
		{
			YAML::Node transform_node = entity_node["Transform"];
			Transform& transform = entity.add<Transform>();
			transform.setPosition(transform_node["Position"].as<vector3D>());
			transform.setOrientation(transform_node["Orientation"].as<quaternionD>());
			transform.setScale(transform_node["Scale"].as<vector3D>());
		}

		if (entity_node["Model"])
		{
			YAML::Node model_node = entity_node["Model"];
			ModelComponent& model = entity.add<ModelComponent>();
			model.mesh = resource_manager->mesh_pool.getResource(model_node["Mesh"].as<std::string>());
			model.material = resource_manager->material_pool.getResource(model_node["Material"].as<std::string>());
		}

		if (entity_node["DirectionalLight"])
		{
			YAML::Node light_node = entity_node["DirectionalLight"];
			DirectionalLight& light = entity.add<DirectionalLight>();
			light.enabled = light_node["enabled"].as<bool>();
			light.color = light_node["color"].as<vector3F>();
			light.intensity = light_node["intensity"].as<float>();
		}

		if (entity_node["PointLight"])
		{
			YAML::Node light_node = entity_node["PointLight"];
			PointLight& light = entity.add<PointLight>();
			light.enabled = light_node["enabled"].as<bool>();
			light.color = light_node["color"].as<vector3F>();
			light.intensity = light_node["intensity"].as<float>();
			light.range = light_node["range"].as<float>();
			light.attenuation = light_node["attenuation"].as<vector2F>();
		}

		if (entity_node["RigidBody"])
		{
			YAML::Node body_node = entity_node["RigidBody"];
			RigidBodyTemplate& rigid_body = entity.add<RigidBodyTemplate>();
			rigid_body.type = (RigidBodyType) body_node["type"].as<int>();
			rigid_body.mass = body_node["mass"].as<float>();
			rigid_body.gravity_enabled = body_node["gravity_enabled"].as<bool>();
			rigid_body.is_allowed_to_sleep = body_node["is_allowed_to_sleep"].as<bool>();
			rigid_body.linear_velocity = body_node["linear_velocity"].as<vector3D>();
			rigid_body.angular_velocity = body_node["angular_velocity"].as<vector3D>();
		}

		if (entity_node["CollisionShape"])
		{
			YAML::Node shape_node = entity_node["CollisionShape"];
			CollisionShapeTemplate& collision_shape = entity.add<CollisionShapeTemplate>();
			collision_shape.type = (ShapeType)shape_node["type"].as<int>();
			switch (collision_shape.type)
			{
			case ShapeType::Box:
				collision_shape.type_data.box_size = shape_node["data"].as<vector3D>();
				break;
			case ShapeType::Sphere:
				collision_shape.type_data.sphere_radius = shape_node["data"].as<double>();
				break;
			case ShapeType::Capsule:
				collision_shape.type_data.capsule_size = shape_node["data"].as<vector2D>();
				break;
			}
		}

		if (entity_node["Children"])
		{
			auto children = entity_node["Children"];
			for (auto child : children)
			{
				entity.addChild(deserializeEntity(child, scene, resource_manager));
			}
		}

		return entity;
	}

	void SceneSerializer::serialize(Scene* scene, const char* file_path)
	{
		YAML::Node scene_node;

		{
			YAML::Node scene_info_node;
			scene_info_node["ambient_light"] = scene->lighting_settings.ambient_light;
			scene_info_node["gamma_correction"] = scene->lighting_settings.gamma_correction;
			scene_node["Lighting"] = scene_info_node;
		}

		//Always write a gravity, not used right now
		{
			YAML::Node physics_node;
			physics_node["gravity"] = vector3F(0.0, 0.0, 0.0);
			scene_node["Physics"] = physics_node;
		}

		scene->registry.each([&](auto entity)
		{
			if (entity != scene->scene_components.handle())
			{
				if (!scene->registry.has<ChildNode>(entity))
				{
					scene_node["Entities"].push_back(serializeEntity(Entity(scene, entity)));
				}
			}
		});

		std::ofstream file_out(file_path);
		file_out << scene_node;
	}

	Scene* SceneSerializer::deserialize(const char* file_path, ResourceManager* resource_manager)
	{
		YAML::Node scene_node = YAML::LoadFile(file_path);

		Scene* scene = new Scene();

		if (scene_node["Lighting"])
		{
			YAML::Node scene_info_node = scene_node["Lighting"];
			scene->lighting_settings.ambient_light = scene_info_node["ambient_light"].as<vector3F>();
			scene->lighting_settings.gamma_correction = scene_info_node["gamma_correction"].as<float>();
		}

		if (scene_node["Entities"])
		{
			auto entities = scene_node["Entities"];
			for (auto entity : entities)
			{
				deserializeEntity(entity, scene, resource_manager);
			}
		}

		return scene;
	}
}