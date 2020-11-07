#pragma once

namespace Genesis
{
	class EntityTest;

	class Hierarchy
	{
	public:
		EntityTest* parent = nullptr;
		vector_set<EntityTest*> children;

		~Hierarchy() 
		{
			for (auto child : this->children)
			{
				delete child;
			}
		};
	};
}