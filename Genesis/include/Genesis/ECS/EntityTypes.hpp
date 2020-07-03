#pragma once

namespace Genesis
{
	template <typename T>
	struct TypeInfo
	{
		constexpr static size_t getHash()
		{
			return typeid(T).hash_code();
		}

		constexpr static size_t getSize()
		{
			return sizeof(T);
		}
	};

	typedef uint64_t EntityId;
	const EntityId InvalidEntity = std::numeric_limits<EntityId>::max();

	typedef uint64_t ComponentId;
	const size_t MAX_COMPONENTS = sizeof(ComponentId) * 8;
	typedef bitset<MAX_COMPONENTS> EntitySignature;

	typedef function<void(void*)> ComponentDeleteFunction;
	struct ComponentInfo
	{
		ComponentId component_id;
		size_t component_size;
		ComponentDeleteFunction component_delete_function;
	};
}