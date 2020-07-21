#pragma once

#include "Genesis/ECS/EntityRegistry.hpp"
#include "Genesis/ECS/EntityPool.hpp"

namespace Genesis
{
	template <class T>
	class EntitySystem
	{
	protected:
		typedef function<void(EntityPool*, const T&)> SystemFunctionPtr;

	public:
		EntitySystem(EntitySignature read_sig, EntitySignature write_sig, EntitySignature exclude_sig, SystemFunctionPtr function_ptr)
		{
			this->read_signature = read_sig;
			this->write_signature = write_sig;
			this->exclude_signature = exclude_signature;
			this->function_ptr = function_ptr;
		};

		void runSystem(EntityWorld* world, const T& data)
		{
			vector<EntityPool*>& pools = *world->getEntityPools(this->read_signature | this->write_signature);

			for (size_t i = 0; i < pools.size(); i++)
			{
				if ((pools[i]->getSignature() & this->exclude_signature).none())
				{
					this->function_ptr(pools[i], data);
				}
			}
		};

	protected:
		EntitySignature read_signature;
		EntitySignature write_signature;
		EntitySignature exclude_signature;
		SystemFunctionPtr function_ptr;
	};
}