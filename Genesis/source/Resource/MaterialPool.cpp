#include "Genesis/Resource/MaterialPool.hpp"

namespace Genesis
{
	MaterialPool::MaterialPool(TexturePool* texture_pool)
	{
		this->texture_pool = texture_pool;
	}

	shared_ptr<Material> MaterialPool::loadResource(const string& key)
	{
		return std::make_shared<Material>(key);
	}
}