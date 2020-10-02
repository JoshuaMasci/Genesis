#pragma once

namespace Genesis
{
	class Resource
	{
	protected:
		const string file_path;

	public:

		Resource(const string& file_path = "")
			:file_path(file_path) {};

		const string& getPath() { return this->file_path; };
	};
}