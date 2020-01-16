#include "ResourceLoaders.hpp"

using namespace Genesis;

#include "Genesis/Core/Log.hpp"
#include <fstream>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

Mesh ObjLoader::loadMesh(RenderingBackend * backend, string mesh_file_path)
{
	struct TexturedVertex
	{
		vector3F pos;
		vector3F normal;
		vector2F uv;
	};

	Mesh mesh;

	tinyobj::attrib_t attrib;
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;
	string warn, err;

	if (tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, mesh_file_path.c_str()))
	{
		vector<TexturedVertex> vertices;
		vector<uint32_t> indices;

		for (const auto& shape : shapes)
		{
			size_t index_offset = 0;
			for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
			{
				int fv = shape.mesh.num_face_vertices[f];

				// Loop over vertices in the face
				for (size_t v = 0; v < fv; v++)
				{
					// access to vertices[i]
					tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
					vector3F position = vector3F(attrib.vertices[3 * idx.vertex_index + 0], attrib.vertices[3 * idx.vertex_index + 1], attrib.vertices[3 * idx.vertex_index + 2]);
					vector3F normal = vector3F(attrib.normals[3 * idx.normal_index + 0], attrib.normals[3 * idx.normal_index + 1], attrib.normals[3 * idx.normal_index + 2]);
					vector2F uv = vector2F(attrib.texcoords[2 * idx.texcoord_index + 0], -attrib.texcoords[2 * idx.texcoord_index + 1]);

					TexturedVertex vertex = { position, normal, uv };

					vertices.push_back(vertex);
					indices.push_back((uint32_t)indices.size());
				}
				index_offset += fv;
			}
		}

		VertexInputDescription vertex_description
		({
			{"in_position", VertexElementType::float_3},
			{"in_normal", VertexElementType::float_3},
			{"in_uv", VertexElementType::float_2}
		});

		mesh.vertex_buffer = backend->createVertexBuffer(vertices.data(), sizeof(TexturedVertex) * vertices.size(), vertex_description);
		mesh.index_buffer = backend->createIndexBuffer(indices.data(), sizeof(uint32_t) * indices.size(), IndexType::uint32);
		mesh.index_count = (uint32_t)indices.size();
	}
	else
	{
		GENESIS_ENGINE_WARN("Can't load Mesh {}", mesh_file_path);
	}

	return mesh;
}

Mesh ObjLoader::loadMesh_CalcTangent(RenderingBackend* backend, string mesh_file_path)
{
	struct TexturedVertex
	{
		vector3F pos;
		vector3F normal;
		vector2F uv;
		vector3F tangent;
	};

	Mesh mesh;

	tinyobj::attrib_t attrib;
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;
	string warn, err;

	if (tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, mesh_file_path.c_str()))
	{
		vector<TexturedVertex> vertices;
		vector<uint32_t> indices;

		for (const auto& shape : shapes)
		{
			size_t index_offset = 0;
			for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
			{
				int fv = shape.mesh.num_face_vertices[f];

				// Loop over vertices in the face
				for (size_t v = 0; v < fv; v++)
				{
					// access to vertices[i]
					tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
					vector3F position = vector3F(attrib.vertices[3 * idx.vertex_index + 0], attrib.vertices[3 * idx.vertex_index + 1], attrib.vertices[3 * idx.vertex_index + 2]);
					vector3F normal = vector3F(attrib.normals[3 * idx.normal_index + 0], attrib.normals[3 * idx.normal_index + 1], attrib.normals[3 * idx.normal_index + 2]);
					vector2F uv = vector2F(attrib.texcoords[2 * idx.texcoord_index + 0], attrib.texcoords[2 * idx.texcoord_index + 1]);

					//Flip the y axis
					uv.y = 1.0f - uv.y;

					TexturedVertex vertex = { position, normal, uv, vector3F(0.0f) };

					vertices.push_back(vertex);
					indices.push_back((uint32_t)indices.size());
				}
				index_offset += fv;
			}
		}

		for (size_t i = 0; i < indices.size(); i += 3)
		{
			uint32_t index_1 = indices[i + 0];
			uint32_t index_2 = indices[i + 1];
			uint32_t index_3 = indices[i + 2];

			vector3F edge_1 = vertices[index_2].pos - vertices[index_1].pos;
			vector3F edge_2 = vertices[index_3].pos - vertices[index_1].pos;

			vector2F uv_1 = vertices[index_2].uv - vertices[index_1].uv;
			vector2F uv_2 = vertices[index_3].uv - vertices[index_1].uv;

			float factor = 1.0f / ((uv_1.x * uv_2.y) - (uv_2.x * uv_1.y));

			vector3F tangent;
			tangent.x = factor * ((uv_2.y * edge_1.x) - (uv_1.y * edge_2.x));
			tangent.y = factor * ((uv_2.y * edge_1.y) - (uv_1.y * edge_2.y));
			tangent.z = factor * ((uv_2.y * edge_1.z) - (uv_1.y * edge_2.z));
			tangent = glm::normalize(tangent);

			vertices[index_1].tangent = tangent;
			vertices[index_2].tangent = tangent;
			vertices[index_3].tangent = tangent;
		}

		VertexInputDescription vertex_description
		({
			{"in_position", VertexElementType::float_3},
			{"in_normal", VertexElementType::float_3},
			{"in_uv", VertexElementType::float_2},
			{"in_tangent", VertexElementType::float_3}
		});

		mesh.vertex_buffer = backend->createVertexBuffer(vertices.data(), sizeof(TexturedVertex) * vertices.size(), vertex_description);
		mesh.index_buffer = backend->createIndexBuffer(indices.data(), sizeof(uint32_t) * indices.size(), IndexType::uint32);
		mesh.index_count = (uint32_t)indices.size();
	}
	else
	{
		GENESIS_ENGINE_WARN("Can't load Mesh {}", mesh_file_path);
	}

	return mesh;
}

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture PngLoader::loadTexture(RenderingBackend * backend, string texture_file_path)
{
	int width, height, tex_channels;
	uint8_t* data = stbi_load(texture_file_path.c_str(), &width, &height, &tex_channels, STBI_rgb_alpha);
	uint64_t data_size = width * height * STBI_rgb_alpha;

	if (data == NULL)
	{
		GENESIS_ENGINE_WARN("Can't load Texture {}", texture_file_path);
		return nullptr;
	}

	Texture texture = backend->createTexture(vector2U((uint32_t)width, (uint32_t)height), (void*)data, data_size);
	stbi_image_free(data);
	return texture;
	return nullptr;
}

Shader ShaderLoader::loadShader(RenderingBackend* backend, string vert_file_path, string frag_file_path)
{
	string vert_data = "";
	string frag_data = "";

	std::ifstream vert_file(vert_file_path, std::ios::ate | std::ios::binary);
	if (vert_file.is_open())
	{
		size_t fileSize = (size_t)vert_file.tellg();
		vert_file.seekg(0);
		vert_data.resize(fileSize);
		vert_file.read(vert_data.data(), vert_data.size());
		vert_file.close();
	}

	std::ifstream frag_file(frag_file_path, std::ios::ate | std::ios::binary);
	if (frag_file.is_open())
	{
		size_t fileSize = (size_t)frag_file.tellg();
		frag_file.seekg(0);
		frag_data.resize(fileSize);
		frag_file.read(frag_data.data(), frag_data.size());
		frag_file.close();
	}

	return backend->createShader(vert_data, frag_data);
}

Shader ShaderLoader::loadShaderSingle(RenderingBackend* backend, string shader_file_path)
{
	return ShaderLoader::loadShader(backend, shader_file_path + ".vert.spv", shader_file_path + ".frag.spv");
}

/*#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

void GLTF_Loader::loadGLTF(RenderingBackend* backend, string file_path)
{
	using namespace tinygltf;

	Model model;
	TinyGLTF loader;
	std::string err;
	std::string warn;

	//bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, argv[1]);
	bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, file_path);

	if (!warn.empty()) 
	{
		printf1("Warn: %s\n", warn.c_str());
	}

	if (!err.empty()) 
	{
		printf1("Err: %s\n", err.c_str());
	}

	if (!ret) 
	{
		printf1("Failed to parse glTF\n");
		return;
	}

	tinygltf::Mesh* mesh = &model.meshes[0];
	tinygltf::Primitive primitive = mesh->primitives[0];
	tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];
	
	int i = 1 + 7;
}*/

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Genesis/Rendering/Model.hpp"


matrix4F toMat4(aiMatrix4x4 matrix)
{
	matrix4F mat4;
	assert(sizeof(matrix4F) == sizeof(aiMatrix4x4));
	memcpy(&mat4, &matrix, sizeof(aiMatrix4x4));
	return glm::transpose(mat4);
}

void addWeight(AnimatedModel::AnimatedModelVertex* vertex, aiVertexWeight* weight, uint8_t bone_id)
{
	for (uint8_t i = 0; i < 4; i++)
	{
		if (vertex->joint_weights[i] == 0.0f)
		{
			vertex->joint_ids[i] = bone_id;
			vertex->joint_weights[i] = weight->mWeight;
			return;
		}
	}

	GENESIS_ENGINE_WARN("Too many weights on vertex");
}

Mesh AssimpLoader::loadMesh(RenderingBackend* backend, string file_path)
{
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(file_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_LimitBoneWeights | aiProcess_CalcTangentSpace | aiProcess_OptimizeGraph);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		//cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
	}

	Mesh animated_mesh;
	matrix4F rotation = toMat4(scene->mRootNode->mChildren[1]->mChildren[0]->mTransformation);

	if (scene->HasMeshes())
	{
		aiMesh* mesh = scene->mMeshes[0];
		vector<AnimatedModel::AnimatedModelVertex> vertices(mesh->mNumVertices);
		vector<uint32_t> indices(mesh->mNumFaces * 3);

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			vertices[i].position.x = mesh->mVertices[i].x;
			vertices[i].position.y = mesh->mVertices[i].y;
			vertices[i].position.z = mesh->mVertices[i].z;
			vertices[i].position = (rotation * vector4F(vertices[i].position, 1.0f));
		
			vertices[i].uv.x = mesh->mTextureCoords[0][i].x;
			vertices[i].uv.y = mesh->mTextureCoords[0][i].y;

			vertices[i].normal.x = mesh->mNormals[i].x;
			vertices[i].normal.y = mesh->mNormals[i].y;
			vertices[i].normal.z = mesh->mNormals[i].z;
			vertices[i].normal = (rotation * vector4F(vertices[i].normal, 1.0f));

			vertices[i].tangent.x = mesh->mTangents[i].x;
			vertices[i].tangent.y = mesh->mTangents[i].y;
			vertices[i].tangent.z = mesh->mTangents[i].z;
			vertices[i].tangent = (rotation * vector4F(vertices[i].tangent, 1.0f));

			vertices[i].bitangent.x = mesh->mBitangents[i].x;
			vertices[i].bitangent.y = mesh->mBitangents[i].y;
			vertices[i].bitangent.z = mesh->mBitangents[i].z;
			vertices[i].bitangent = (rotation * vector4F(vertices[i].bitangent, 1.0f));

			vertices[i].joint_ids = vector4U(0);
			vertices[i].joint_weights = vector4F(0.0f);
		}

		for (unsigned int i = 0; i < mesh->mNumBones; i++)
		{
			aiBone* bone = mesh->mBones[i];

			for (unsigned int j = 0; j < bone->mNumWeights; j++)
			{
				aiVertexWeight* weight = &bone->mWeights[j];
				addWeight(&vertices[weight->mVertexId], weight, i);
			}
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			uint32_t index = i * 3;

			indices[index + 0] = face.mIndices[0];
			indices[index + 1] = face.mIndices[1];
			indices[index + 2] = face.mIndices[2];
		}

		for (size_t i = 0; i < vertices.size(); i++)
		{
			vertices[i].joint_weights = glm::normalize(vertices[i].joint_weights);
		}

		animated_mesh.vertex_buffer = backend->createVertexBuffer(vertices.data(), sizeof(AnimatedModel::AnimatedModelVertex) * vertices.size(), AnimatedModel::getVertexDescription());
		animated_mesh.index_buffer = backend->createIndexBuffer(indices.data(), sizeof(uint32_t) * indices.size(), IndexType::uint32);
		animated_mesh.index_count = (uint32_t)indices.size();
	}

	return animated_mesh;
}

Skeleton* AssimpLoader::loadSkeleton(RenderingBackend* backend, string file_path)
{
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(file_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_LimitBoneWeights | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		//cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
	}

	Skeleton* skeleton = nullptr;
	aiMatrix4x4 rotation = scene->mRootNode->mChildren[0]->mTransformation;


	if (scene->HasMeshes())
	{
		aiMesh* mesh = scene->mMeshes[0];

		skeleton = new Skeleton(mesh->mNumBones);

		for (unsigned int i = 0; i < mesh->mNumBones; i++)
		{
			Bone* bone = skeleton->getBone(i);
			bone->name = mesh->mBones[i]->mName.C_Str();
			bone->inverse_bind_transform = glm::inverse(toMat4(rotation * mesh->mBones[i]->mOffsetMatrix));
		}

		for (unsigned int i = 0; i < mesh->mNumBones; i++)
		{
			Bone* bone = skeleton->getBone(i);
			aiNode* node = scene->mRootNode->FindNode(mesh->mBones[i]->mName);

			aiVector3D ai_position;
			aiQuaternion ai_orientation;
			aiVector3D ai_scale;
			aiMatrix4x4 ai_matrix = rotation * node->mTransformation;
			ai_matrix.Decompose(ai_scale, ai_orientation, ai_position);

			vector3F position = vector3F(ai_position.x, ai_position.y, ai_position.z);
			quaternionF orientation = quaternionF(ai_orientation.w, ai_orientation.x, ai_orientation.y, ai_orientation.z);
			vector3F scale = vector3F(ai_scale.x, ai_scale.y, ai_scale.z);

			bone->local_transform = TransformF(position, orientation, scale);

			size_t bone_count = 0;

			for (unsigned int j = 0; j < node->mNumChildren; j++)
			{
				if (skeleton->getBone(node->mChildren[j]->mName.C_Str()) != nullptr)
				{
					bone_count++;
				}
			}

			bone->child_bones.resize(bone_count);
			size_t bone_index = 0;
			for (unsigned int j = 0; j < node->mNumChildren; j++)
			{
				Bone* child_bone = skeleton->getBone(node->mChildren[j]->mName.C_Str());
				if (child_bone != nullptr)
				{
					bone->child_bones[bone_index] = child_bone;
					bone_index++;
				}
			}
		}

	}

	return skeleton;
}
