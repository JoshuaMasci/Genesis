#pragma once

#include "Genesis/Core/Transform.hpp"

namespace Genesis
{
	class WorldTransform : public TransformD
	{
		//No changes, just a wrapper for TransformD
	public:
		WorldTransform() :TransformD() {};
		WorldTransform(const TransformD& transform) :TransformD(transform) {};
	};

	class LocalTransform : public TransformD
	{
		//No changes, just a wrapper for TransformD
	public:
		LocalTransform() :TransformD() {};
		LocalTransform(const TransformD& transform) :TransformD(transform) {};
	};

	class RootTransform : public TransformD
	{
		//No changes, just a wrapper for TransformD
	public:
		RootTransform() :TransformD() {};
		RootTransform(const TransformD& transform) :TransformD(transform) {};
	};
}