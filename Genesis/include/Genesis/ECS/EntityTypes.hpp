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

	typedef uint32_t EntityId;
	typedef uint32_t ComponentId;

	const EntityId InvalidEntity = std::numeric_limits<EntityId>::max();

	typedef function<void(void*)> ComponentDeleteFunction;
	struct ComponentInfo
	{
		ComponentId component_id;
		size_t component_size;
		ComponentDeleteFunction component_delete_function;
	};
}