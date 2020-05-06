#pragma once

//LegacyBackend only for now
#include "Genesis/LegacyBackend/LegacyBackend.hpp"

#include "tiny_gltf.h"

namespace Genesis
{
	struct GltfNode;

	struct GltfVertex
	{
		vector3F position;
		vector3F normal;
		vector2F uv0;
		vector2F uv1;
		vector4F joint0;
		vector4F weight0;
	};

	struct BoundingBox
	{
		vector3F min;
		vector3F max;
		bool valid = false;
		BoundingBox() {};
		BoundingBox(vector3F min, vector3F max) : min(min), max(max) {};
	};

	struct GltfMaterial
	{
		enum AlphaMode { ALPHAMODE_OPAQUE, ALPHAMODE_MASK, ALPHAMODE_BLEND };
		AlphaMode alpha_mode = ALPHAMODE_OPAQUE;
		float alpha_cutoff = 1.0f;
		float metallic_factor = 1.0f;
		float roughness_factor = 1.0f;
		vector4F base_color_factor = vector4F(1.0f);
		vector4F emissive_factor = vector4F(1.0f);

		Texture2D base_color_texture;
		Texture2D metallic_roughness_texture;
		Texture2D normal_texture;
		Texture2D occlusion_texture;
		Texture2D emissive_texture;
		struct TexCoordSets
		{
			uint8_t base_color = 0;
			uint8_t metallic_toughness = 0;
			uint8_t specular_glossiness = 0;
			uint8_t normal = 0;
			uint8_t occlusion = 0;
			uint8_t emissive = 0;
		} tex_coord_sets;

		struct Extension
		{
			Texture2D specular_glossiness_texture;
			Texture2D diffuse_texture;
			vector4F diffuse_factor = vector4F(1.0f);
			vector3F specular_factor = vector3F(0.0f);
		} extension;

		struct PbrWorkflows
		{
			bool metallic_roughness = true;
			bool specular_glossiness = false;
		} pbrWorkflows;
	};

	struct GltfMeshPrimitive
	{
		uint32_t first_index;
		uint32_t index_count;
		uint32_t vertex_count;
		GltfMaterial* material;
		BoundingBox bounding_box;
	};

	struct GltfMesh
	{
		VertexBuffer vertices;
		IndexBuffer indices;

		vector<GltfMeshPrimitive> primitives;
		BoundingBox bounding_box;
		BoundingBox axis_aligned_bounding_box;
	};

	struct GltfSkin
	{
		string name;
		uint32_t skeleton_root_index;
		vector<uint32_t> joints;
		vector<matrix4F> inverse_bind_matrices;
	};

	struct GltfAnimationSampler
	{
		enum InterpolationType { LINEAR, STEP, CUBICSPLINE };
		InterpolationType interpolation;
		vector<float> inputs;
		vector<vector4F> outputs_vector4;
	};

	struct GltfAnimationChannel
	{
		enum PathType { TRANSLATION, ROTATION, SCALE };
		PathType path;
		uint32_t node_index;
		uint32_t sampler_index;
	};

	struct GltfAnimation
	{
		string name;
		vector<GltfAnimationSampler> samplers;
		vector<GltfAnimationChannel> channels;
		float start_time = std::numeric_limits<float>::max();
		float end_time = std::numeric_limits<float>::min();
	};

	struct GltfNode
	{
		string name;
		uint32_t index;

		TransformF local_transform;

		vector<uint32_t> child_indices;

		GltfMesh *mesh = nullptr;
		GltfSkin *skin = nullptr;
	};

	class GltfModel
	{
	public:
		GltfModel(LegacyBackend* legacy_backend, tinygltf::Model& gltfModel);
		~GltfModel();

	protected:
		LegacyBackend* backend;

		vector<Texture2D> textures;
		vector<GltfMaterial> materials;
		vector<GltfMesh> meshes;

		vector<GltfNode> node_storage;

		vector<GltfSkin> skins;
		vector<GltfAnimation> animations;
		vector<string> extensions;

		void loadTextures(tinygltf::Model& gltfModel);
		void unloadTextures();

		void loadMaterials(tinygltf::Model& gltfModel);
		void unloadMaterials();

		void loadMeshes(tinygltf::Model& gltfModel);
		void unloadMeshes();

		void loadSkins(tinygltf::Model& gltfModel);
		void unloadSkins();

		void loadNodes(tinygltf::Model& gltfModel);
		void unloadNodes();

		void loadAnimations(tinygltf::Model& gltfModel);
		void unloadAnimations();
	};

	class GltfLoader
	{
	public:
		//static GltfModel& loadModel(string filename, string file_path);
	};

}