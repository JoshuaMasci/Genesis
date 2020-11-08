#include "Genesis/World/Hierarchy.hpp"


#include "Genesis/World/Entity.hpp"

namespace Genesis
{
	namespace Experimental
	{
		Hierarchy::~Hierarchy()
		{
			for (auto child : this->children)
			{
				delete child;
			}
		}
	}
}