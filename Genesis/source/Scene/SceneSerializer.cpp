#include "Genesis/Scene/SceneSerializer.hpp"

#include <fstream>
#include "Genesis/Core/Yaml.hpp"

#include "Genesis/Scene/Entity.hpp"

#include "Genesis/Rendering/SceneInfo.hpp"
#include "Genesis/Physics/PhysicsWorld.hpp"

#include "Genesis/Component/NameComponent.hpp"
#include "Genesis/Component/TransformComponent.hpp"
#include "Genesis/Component/ModelComponent.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Lights.hpp"
#include "Genesis/Physics/RigidBody.hpp"
#include "Genesis/Physics/CollisionShape.hpp"

#include "Genesis/Resource/ResourceManager.hpp"

namespace Genesis
{
	YAML::Node serialize_entity_prototype(EntityPrototype* entity)
	{
		YAML::Node entity_node;
		entity_node["Entity_UUID"] = 0;//TODO
		entity_node["Name"] = entity->get_name();

		{
			Transform& transform = entity->get_transform();
			YAML::Node transform_node;
			transform_node["Position"] = transform.getPosition();
			transform_node["Orientation"] = transform.getOrientation();
			transform_node["Scale"] = transform.getScale();
			entity_node["Transform"] = transform_node;
		}

		if (entity->has_component<ModelComponent>())
		{
			ModelComponent* model = entity->get_component<ModelComponent>();
			YAML::Node model_node;
			model_node["Mesh"] = model->mesh->getName();
			model_node["Material"] = model->material->getName();
			entity_node["Model"] = model_node;
		}


		if (entity->has_component<DirectionalLight>())
		{
			DirectionalLight* light = entity->get_component<DirectionalLight>();
			YAML::Node light_node;
			light_node["enabled"] = light->enabled;
			light_node["color"] = light->color;
			light_node["intensity"] = light->intensity;
			entity_node["DirectionalLight"] = light_node;
		}

		if (entity->has_component<PointLight>())
		{
			PointLight* light = entity->get_component<PointLight>();
			YAML::Node light_node;
			light_node["enabled"] = light->enabled;
			light_node["color"] = light->color;
			light_node["intensity"] = light->intensity;
			light_node["range"] = light->range;
			light_node["attenuation"] = light->attenuation;
			entity_node["PointLight"] = light_node;
		}

		if (entity->has_component<RigidBody>())
		{
			RigidBody* rigid_body = entity->get_component<RigidBody>();
			YAML::Node body_node;
			body_node["type"] = (int)rigid_body->getType();
			body_node["mass"] = rigid_body->getMass();
			body_node["gravity_enabled"] = rigid_body->getGravityEnabled();
			body_node["is_allowed_to_sleep"] = rigid_body->getIsAllowedToSleep();
			body_node["linear_velocity"] = rigid_body->getLinearVelocity();
			body_node["angular_velocity"] = rigid_body->getAngularVelocity();
			entity_node["RigidBody"] = body_node;
		}

		if (entity->has_component<CollisionShape>())
		{
			CollisionShape* collision_shape = entity->get_component<CollisionShape>();
			YAML::Node shape_node;
			shape_node["type"] = (int)collision_shape->type;
			switch (collision_shape->type)
			{
			case CollisionShapeType::Box:
				shape_node["data"] = collision_shape->type_data.box_size;
				break;
			case CollisionShapeType::Sphere:
				shape_node["data"] = collision_shape->type_data.sphere_radius;
				break;
			case CollisionShapeType::Capsule:
				shape_node["data"] = collision_shape->type_data.capsule_size;
				break;
			}

			entity_node["CollisionShape"] = shape_node;
		}

		for (auto child: entity->get_children())
		{
			entity_node["Children"].push_back(serialize_entity_prototype(child));
		}

		return entity_node;
	}

	void SceneSerializer::serialize_prototype(ScenePrototype* scene, const char* file_path)
	{
		YAML::Node scene_node;

		/*if (scene->has_component<SceneInfo>())
		{
			SceneInfo* scene_info = scene->get_component<SceneInfo>();
			YAML::Node scene_info_node;
			scene_info_node["ambient_light"] = scene_info->ambient_light;
			scene_node["SceneInfo"] = scene_info_node;
		}

		if (scene->has_component<PhysicsWorld>())
		{
			PhysicsWorld* physics = scene->get_component<PhysicsWorld>();
			YAML::Node physics_node;
			physics_node["gravity"] = physics->getGravity();
			scene_node["PhysicsWorld"] = physics_node;
		}*/

		for (auto entity: scene->get_entities())
		{
			scene_node["Entities"].push_back(serialize_entity_prototype(entity));
		}

		std::ofstream file_out(file_path);
		file_out << scene_node;
		file_out.close();
	}

	EntityPrototype* deserialize_entity_prototype(YAML::Node& entity_node, ResourceManager* resource_manager)
	{
		EntityPrototype* entity = new EntityPrototype("Yaml Entity");

		if (entity_node["Name"])
		{
			entity->set_name(entity_node["Name"].as<std::string>());
		}

		if (entity_node["Transform"])
		{
			YAML::Node transform_node = entity_node["Transform"];
			TransformD transform;
			transform.setPosition(transform_node["Position"].as<vector3D>());
			transform.setOrientation(transform_node["Orientation"].as<quaternionD>());
			transform.setScale(transform_node["Scale"].as<vector3D>());
			entity->set_transform(transform);
		}

		if (entity_node["Model"])
		{
			YAML::Node model_node = entity_node["Model"];
			ModelComponent* model = entity->add_component<ModelComponent>();
			model->mesh = resource_manager->mesh_pool.getResource(model_node["Mesh"].as<std::string>());
			model->material = resource_manager->material_pool.getResource(model_node["Material"].as<std::string>());
		}

		if (entity_node["DirectionalLight"])
		{
			YAML::Node light_node = entity_node["DirectionalLight"];
			DirectionalLight* light = entity->add_component<DirectionalLight>();
			light->enabled = light_node["enabled"].as<bool>();
			light->color = light_node["color"].as<vector3F>();
			light->intensity = light_node["intensity"].as<float>();
		}

		if (entity_node["PointLight"])
		{
			YAML::Node light_node = entity_node["PointLight"];
			PointLight* light = entity->add_component<PointLight>();
			light->enabled = light_node["enabled"].as<bool>();
			light->color = light_node["color"].as<vector3F>();
			light->intensity = light_node["intensity"].as<float>();
			light->range = light_node["range"].as<float>();
			light->attenuation = light_node["attenuation"].as<vector2F>();
		}

		if (entity_node["RigidBody"])
		{
			YAML::Node body_node = entity_node["RigidBody"];
			RigidBody* rigid_body = entity->add_component<RigidBody>();
			rigid_body->setType((RigidBodyType)body_node["type"].as<int>());
			rigid_body->setMass(body_node["mass"].as<float>());
			rigid_body->setGravityEnabled(body_node["gravity_enabled"].as<bool>());
			rigid_body->setIsAllowedToSleep(body_node["is_allowed_to_sleep"].as<bool>());
			rigid_body->setLinearVelocity(body_node["linear_velocity"].as<vector3D>());
			rigid_body->setAngularVelocity(body_node["angular_velocity"].as<vector3D>());
		}

		if (entity_node["CollisionShape"])
		{
			YAML::Node shape_node = entity_node["CollisionShape"];
			CollisionShape* collision_shape = entity->add_component<CollisionShape>();
			collision_shape->type = (CollisionShapeType)shape_node["type"].as<int>();
			switch (collision_shape->type)
			{
			case CollisionShapeType::Box:
				collision_shape->type_data.box_size = shape_node["data"].as<vector3D>();
				break;
			case CollisionShapeType::Sphere:
				collision_shape->type_data.sphere_radius = shape_node["data"].as<double>();
				break;
			case CollisionShapeType::Capsule:
				collision_shape->type_data.capsule_size = shape_node["data"].as<vector2D>();
				break;
			}
		}

		if (entity_node["Children"])
		{
			auto children = entity_node["Children"];
			for (auto child : children)
			{
				entity->add_child(deserialize_entity_prototype(child, resource_manager));
			}
		}

		return entity;
	}

	ScenePrototype* SceneSerializer::deserialize_prototype(const char* file_path, ResourceManager* resource_manager)
	{
		YAML::Node scene_node = YAML::LoadFile(file_path);

		ScenePrototype* scene = new ScenePrototype();

		if (scene_node["SceneInfo"])
		{
			YAML::Node scene_info_node = scene_node["SceneInfo"];
			SceneInfo* scene_info = scene->add_component<SceneInfo>();
			scene_info->ambient_light = scene_info_node["ambient_light"].as<vector3F>();
		}

		if (scene_node["PhysicsWorld"])
		{
			YAML::Node physics_node = scene_node["PhysicsWorld"];
			//PhysicsWorld* physics = scene->add_component<PhysicsWorld>(physics_node["gravity"].as<vector3D>());
		}

		if (scene_node["Entities"])
		{
			auto entities = scene_node["Entities"];
			for (auto entity : entities)
			{
				scene->add_entity(deserialize_entity_prototype(entity, resource_manager));
			}
		}

		return scene;
	}
}