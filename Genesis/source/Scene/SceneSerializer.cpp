#include "Genesis/Scene/SceneSerializer.hpp"

#include <fstream>

#define YAML_CPP_DLL
#include <yaml-cpp/yaml.h>

#include "Genesis/Scene/Entity.hpp"

#include "Genesis/Rendering/SceneInfo.hpp"

#include "Genesis/Component/NameComponent.hpp"
#include "Genesis/Component/TransformComponent.hpp"
#include "Genesis/Component/ModelComponent.hpp"

#include "Genesis/Resource/MeshPool.hpp"
#include "Genesis/Resource/MaterialPool.hpp"

namespace YAML 
{
	template<>
	struct convert<Genesis::vector3D> 
	{
		static Node encode(const Genesis::vector3D& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, Genesis::vector3D& rhs)
		{
			if (!node.IsSequence() || node.size() != 3) 
			{
				return false;
			}

			rhs.x = node[0].as<double>();
			rhs.y = node[1].as<double>();
			rhs.z = node[2].as<double>();
			return true;
		}
	};

	template<>
	struct convert<Genesis::quaternionD>
	{
		static Node encode(const Genesis::quaternionD& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, Genesis::quaternionD& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
			{
				return false;
			}

			rhs.x = node[0].as<double>();
			rhs.y = node[1].as<double>();
			rhs.z = node[2].as<double>();
			rhs.w = node[3].as<double>();
			return true;
		}
	};
}

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

		for (auto child_handle : entity.children())
		{
			entity_node["Children"].push_back(serializeEntity(Entity(entity.get_scene(), child_handle)));
		}

		return entity_node;
	}

	Entity deserializeEntity(YAML::Node& entity_node, Scene* scene, MeshPool* mesh_pool, MaterialPool* material_pool)
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
			model.mesh = mesh_pool->getResource(model_node["Mesh"].as<std::string>());
			model.material = material_pool->getResource(model_node["Material"].as<std::string>());
			entity.add_or_replace<WorldTransform>();
		}

		if (entity_node["Children"])
		{
			auto children = entity_node["Children"];
			for (auto child : children)
			{
				entity.addChild(deserializeEntity(child, scene, mesh_pool, material_pool));
			}
		}

		return entity;
	}

	void SceneSerializer::serialize(Scene* scene, const char* file_path)
	{
		YAML::Node scene_node;

		//TODO: write scene_components

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

	Scene* SceneSerializer::deserialize(const char* file_path, MeshPool* mesh_pool, MaterialPool* material_pool)
	{
		YAML::Node scene_node = YAML::LoadFile(file_path);

		Scene* scene = new Scene();

		//TODO: write scene_components
		scene->scene_components.add<SceneInfo>();

		if (scene_node["Entities"])
		{
			auto entities = scene_node["Entities"];
			for (auto entity : entities)
			{
				deserializeEntity(entity, scene, mesh_pool, material_pool);
			}
		}

		return scene;
	}

	/*void SceneSerializer::addSerializeFuncions(EntitySerializeFunction serialize, EntityDeserializeFunction deserialize)
{
	if (serialize != nullptr)
	{
		this->serialize_functions.push_back(serialize);
	}

	if (deserialize != nullptr)
	{
		this->deserialize_functions.push_back(deserialize);
	}
}*/
}