#pragma once

#include "Genesis/Core/Transform.hpp"

namespace Genesis
{
	typedef TransformD Transform;

	class TransformBase
	{
	protected:
		Transform transform;
		bool has_changed = false;
	public:
		bool setTransform(const Transform& new_transform)
		{
			this->has_changed = (this->transform != new_transform);
			if (this->has_changed)
			{
				this->transform = new_transform;
			}
			return this->has_changed;
		}

		Transform getTransform()
		{
			return this->transform;
		}

		bool hasChanged()
		{
			return this->has_changed;
		}
	};

	class RootTransform : public TransformBase {};
	class WorldTransform : public TransformBase {};
}