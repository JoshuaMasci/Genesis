#pragma once

namespace Genesis
{
	class Entity;

	class Hierarchy
	{
	public:
		~Hierarchy();

		Entity* parent = nullptr;
		vector_set<Entity*> children;
	};
}
