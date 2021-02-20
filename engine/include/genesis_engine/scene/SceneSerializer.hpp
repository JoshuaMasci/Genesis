#pragma once

#include "genesis_engine/scene/Scene.hpp"

namespace genesis_engine
{
	class ResourceManager;

	//typedef void(*EntitySerializeFunction)(Entity, void*);
	//typedef void(*EntityDeserializeFunction)(Entity, void*);

	class SceneSerializer
	{
	public:

		void serialize(Scene* scene, const char* file_path);
		Scene* deserialize(const char* file_path, ResourceManager* resource_manager);

		//void addSerializeFuncions(EntitySerializeFunction serialize, EntityDeserializeFunction deserialize);
	protected:
		//vector<EntitySerializeFunction> serialize_functions;
		//vector<EntityDeserializeFunction> deserialize_functions;
	};
}