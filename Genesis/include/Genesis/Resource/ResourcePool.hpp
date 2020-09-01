#pragma once

namespace Genesis
{
	template <class key_type, class resource_type>
	class ResourcePool
	{
protected:
		struct ResourceInternal
		{
			resource_type resource;
			uint16_t using_count;
		};

		bool hasResource(key_type key)
		{
			return this->resources.find(key) != this->resources.end();
		};

		virtual void loadResource(const key_type& key) = 0;
		virtual void unloadResource(const key_type& key) = 0;

		flat_hash_map<key_type, ResourceInternal> resources;
	
	public:
		virtual ~ResourcePool()
		{

		};

		resource_type getResource(const key_type& key)
		{
			if (!this->hasResource(key))
			{
				this->loadResource(key);
			}

			this->resources[key].using_count++;
			return this->resources[key].resource;
		};

		void freeResource(const key_type& key)
		{
			if (this->hasResource(key))
			{
				this->resources[key].using_count--;

				if (this->resources[key].using_count == 0)
				{
					this->unloadResource(key);
					this->resources.erase(key);
				}
			}
			else
			{
				GENESIS_ENGINE_ERROR("Tried to free unloaded resource!");
			}
		};
	};
}