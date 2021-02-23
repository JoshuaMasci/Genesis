#pragma once

namespace genesis
{
	class Resource
	{
	protected:
		const string name;

	public:
		Resource(const string& name = "")
			:name(name) {};

		const string& getName() { return this->name; };
	};
}