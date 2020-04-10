#include <iostream>

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include <jsoncons/json.hpp>
using jsoncons::json;

using namespace std;

void readWholeFile(const string& filename, string& data)
{
	ifstream file(filename, ios::ate);
	if (file.is_open())
	{
		size_t fileSize = (size_t)file.tellg();
		file.seekg(0);
		data.resize(fileSize);
		data.assign((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));
		file.close();
	}
	else
	{
		printf("Failed to open file: %s\n", filename.c_str());
		exit(1);
	}
}

size_t incrementIndices(const vector<size_t>& element_counts, vector<size_t>& element_indices)
{
	size_t carry = 1;
	for (size_t i = 0; i < element_indices.size(); i++)
	{
		if (carry == 1)
		{
			element_indices[i] += carry;

			if (element_indices[i] == element_counts[i])
			{
				carry = 1;
				element_indices[i] = 0;
			}
			else
			{
				carry = 0;
			}
		}
		else
		{
			carry = 0;
			break;
		}
	}

	return carry;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("Error: invalid input\n");
		printf("usage: ShaderPermutation input_file output_directory\n");
		exit(1);
	}

	string filename(argv[1]);
	string output_path(argv[2]);
	
	ifstream in_file(filename, ios::ate);
	if (!in_file.is_open())
	{
		printf("Error: failed to open %s\n", filename.c_str());
		exit(1);
	}

	json json_file = json::parse(in_file);

	in_file.close();

	//Error check
	{
		if (!json_file.contains("output_name_pre"))
		{
			printf("Error: need output_name_pre\n");
			exit(1);
		}

		if (!json_file.contains("output_name_post"))
		{
			printf("Error: need output_name_post\n");
			exit(1);
		}

		if (!json_file.contains("file_header"))
		{
			printf("Error: need file_header\n");
			exit(1);
		}

		if (!json_file.contains("file_tail"))
		{
			printf("Error: need file_tail\n");
			exit(1);
		}

		if (!json_file.contains("elements"))
		{
			printf("Error: need elements\n");
			exit(1);
		}
	}

	printf("Shader Permutation Generator\n");
	printf("File: %s\n", filename.c_str());
	printf("Output Directory: %s\n", output_path.c_str());

	//Read initial
	string output_name_pre = json_file.at("output_name_pre").as_string();
	string output_name_post = json_file.at("output_name_post").as_string();

	printf("Output file name format: %s\n", (output_name_pre + "***" + output_name_post).c_str());

	string file_header_name = json_file.at("file_header").as_string();
	string file_header;
	readWholeFile(file_header_name, file_header);

	string file_tail_name = json_file.at("file_tail").as_string();
	string file_tail;
	readWholeFile(file_tail_name, file_tail);

	printf("Header: %s\n", file_header_name.c_str());
	printf("Tail: %s\n", file_tail_name.c_str());

	vector<string> element_names;
	{
		json elements = json_file["elements"];
		element_names.resize(elements.size());

		for (size_t i = 0; i < element_names.size(); i++)
		{
			element_names[i] = elements[i].as_string();
		}
	}

	//Create filename tables
	vector<vector<string>> elements(element_names.size());
	vector<size_t> element_count(element_names.size());

	for (size_t i = 0; i < elements.size(); i++)
	{
		printf("Element %zu %s: ", i, element_names[i].c_str());

		if (!json_file.contains(element_names[i]))
		{
			printf("Error: can't load element: %s\n", element_names[i].c_str());
			exit(1);
		}

		json element_list = json_file[element_names[i]];
		elements[i].resize(element_list.size());
		for (size_t j = 0; j < element_list.size(); j++)
		{
			string element_file_name = element_list[j].as_string();
			printf(" %s", element_file_name.c_str());
			readWholeFile(element_file_name, elements[i][j]);

			if (j != (element_list.size() - 1))
			{
				printf(",");
			}
		}

		element_count[i] = element_list.size();

		printf("\n");
	}

	//Loop though all permutations
	vector<size_t> element_indices(elements.size(), 0);

	size_t permutation_count = 0;

	string permutation_index;
	string file_name;
	do
	{
		permutation_index.clear();

		for (size_t i = 0; i < element_indices.size(); i++)
		{
			permutation_index += to_string(element_indices[i]);
		}

		filename = output_path + output_name_pre + permutation_index + output_name_post;

		//Create files
		ofstream out_file(filename);

		if (!out_file.is_open())
		{
			printf("Error: can't open file: %s\n", filename.c_str());
			exit(1);
		}

		out_file << file_header << '\n' << '\n';

		for (size_t i = 0; i < element_indices.size(); i++)
		{
			size_t elment_index = element_indices[i];
			out_file << elements[i][elment_index] << '\n' << '\n';
		}

		out_file << file_tail;
		out_file.close();

		permutation_count++;
	} while (incrementIndices(element_count, element_indices) == 0);

	printf("\n");
	printf("Permutation Generated: %zu\n", permutation_count);

	return 0;
}