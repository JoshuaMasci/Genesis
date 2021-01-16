#pragma once

#include "Genesis/Scene/Scene.hpp"
#include "Genesis/Scene/ScenePrototype.hpp"

namespace Genesis
{
	class ResourceManager;

	//typedef void(*EntitySerializeFunction)(Entity, void*);
	//typedef void(*EntityDeserializeFunction)(Entity, void*);

	class SceneSerializer
	{
	public:
		void serialize_prototype(ScenePrototype* scene, const char* file_path);
		ScenePrototype* deserialize_prototype(const char* file_path, ResourceManager* resource_manager);
	};
}