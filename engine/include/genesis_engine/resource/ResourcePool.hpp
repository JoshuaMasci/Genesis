#pragma once

namespace genesis_engine
{
	template <class key_type, class resource_type>
	class ResourcePool
	{
	protected:
		flat_hash_map<key_type, weak_ptr<resource_type>> resources;

		virtual shared_ptr<resource_type> loadResource(const key_type& key) = 0;

	public:
		shared_ptr<resource_type> getResource(const key_type& key)
		{
			shared_ptr<resource_type> resource;

			auto& resource_it = this->resources.find(key);

			if (resource_it == this->resources.end() || resource_it->second.expired())
			{
				resource = this->loadResource(key);

				if (!resource)
				{
					return nullptr;
				}

				this->resources[key] = resource;
			}
			else
			{
				resource = resource_it->second.lock();
			}

			return resource;
		}
	};
}