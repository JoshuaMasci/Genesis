#include "genesis_engine/scene/Scene.hpp"

#include "genesis_engine/component/NameComponent.hpp"
#include "genesis_engine/scene/Entity.hpp"

namespace genesis_engine
{
	Scene::Scene()
	{
		this->scene_components = SceneComponents(&this->registry, this->registry.create());
		this->scene_components.add<NameComponent>("Scene Components");
	}

	Entity Scene::createEntity(const char* name)
	{
		Entity entity = Entity(this, this->registry.create());
		entity.add<NameComponent>(name);
		return entity;
	}

	void Scene::destoryEntity(Entity entity)
	{
		for (EntityHandle child : entity.children())
		{
			this->destoryEntity(Entity(this, child));
		}

		this->registry.destroy(entity.handle());
	}

	void Scene::addChild(Entity parent, Entity child)
	{
		HierarchyUtils::addChild(this->registry, parent.handle(), child.handle());
	}

	void Scene::removeChild(Entity parent, Entity child)
	{
		HierarchyUtils::removeChild(this->registry, parent.handle(), child.handle());
	}

	void Scene::initialize_scene()
	{
		
	}

	void Scene::deinitialize_scene()
	{

	}
}