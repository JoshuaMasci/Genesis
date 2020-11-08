#pragma once

namespace Genesis
{
	namespace Experimental
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
}
