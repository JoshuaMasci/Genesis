#pragma once

namespace Genesis
{
	template <class key_type, class resource_type>
	class ResourcePool
	{
protected:
		struct ResourceInfo
		{
			key_type key;
			uint16_t using_count;
		};

		virtual resource_type loadResource(const key_type& key) = 0;
		virtual void unloadResource(resource_type resource) = 0;

		flat_hash_map<key_type, resource_type> resources;
		flat_hash_map<resource_type, ResourceInfo> resource_info;

	public:
		virtual ~ResourcePool()
		{
			//TODO delete resources
		};

		resource_type getResource(const key_type& key)
		{
			if (!this->hasResource(key))
			{
				this->resources[key] = this->loadResource(key);
				this->resource_info[this->resources[key]] = {key, 0};
			}

			resource_type& resource = this->resources[key];
			this->resource_info[resource].using_count++;
			return resource;
		};

		void freeResource(const resource_type& resource)
		{
			if (this->hasResource(resource))
			{
				ResourceInfo& resource_info = this->resource_info[resource];
				resource_info.using_count--;

				if (resource_info.using_count == 0)
				{
					this->resources.erase(resource_info.key);
					this->resource_info.erase(resource);
					this->unloadResource(resource);
				}
			}
			else
			{
				GENESIS_ENGINE_ERROR("Tried to free unloaded resource!");
			}
		};

		ResourceInfo& getResourceInfo(const resource_type& resource)
		{
			return this->resource_info[resource];
		};

		bool hasResource(key_type key)
		{
			return this->resources.find(key) != this->resources.end();
		};

		bool hasResource(resource_type resource)
		{
			return this->resource_info.find(resource) != this->resource_info.end();
		};
	};

	template <class key_type, class resource_type>
	class ResourcePoolNew
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