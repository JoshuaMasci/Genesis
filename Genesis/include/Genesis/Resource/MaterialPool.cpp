#include "MaterialPool.hpp"

#include "Genesis/Rendering/ResourceLoaders.hpp"

using namespace Genesis;

MaterialPool::MaterialPool(RenderingBackend* backend)
{
	this->backend = backend;
}

void MaterialPool::loadResource(string key)
{
	//this->resources[key].resource = MaterialLoader::loadMaterial(this->backend, key);
	this->resources[key].using_count = 0;
}

void MaterialPool::unloadResource(string key)
{
	ResourceInternal resource = this->resources[key];

	

	this->resources.erase(key);
}
