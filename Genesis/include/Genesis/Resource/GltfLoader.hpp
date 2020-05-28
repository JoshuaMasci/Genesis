#pragma once

//LegacyBackend only for now
#include "Genesis/LegacyBackend/LegacyBackend.hpp"

#include "Genesis/Resource/PbrMaterial.hpp"
#include "Genesis/Resource/PbrMesh.hpp"
#include "Genesis/Resource/Animation.hpp"
#include "Genesis/Resource/Skeleton.hpp"

#include "tiny_gltf.h"

namespace Genesis
{
	struct GltfNode;

	/*struct GltfVertex
	{
		vector3F position;
		vector3F normal;
		vector2F uv0;
		vector2F uv1;
		vector4F joint0;
		vector4F weight0;
	};*/

	struct GltfBoundingBox
	{
		vector3F min;
		vector3F max;
		GltfBoundingBox() {};
		GltfBoundingBox(vector3F min, vector3F max) : min(min), max(max) {};
	};

	/*struct GltfMaterial
	{
		bool double_sided = false;

		enum AlphaMode { ALPHAMODE_OPAQUE, ALPHAMODE_MASK, ALPHAMODE_BLEND };
		AlphaMode alpha_mode = ALPHAMODE_OPAQUE;
		float alpha_cutoff = 1.0f;

		vector4F albedo_factor = vector4F(1.0f);
		vector2F metallic_roughness_factor = vector2F(1.0f);
		vector4F emissive_factor = vector4F(1.0f);

		Texture2D albedo_texture;
		Texture2D normal_texture;
		Texture2D metallic_roughness_texture;
		Texture2D occlusion_texture;
		Texture2D emissive_texture;
		struct TexCoordSets
		{
			int8_t albedo = -1;
			int8_t normal = -1;
			int8_t metallic_roughness = -1;
			int8_t occlusion = -1;
			int8_t emissive = -1;
		} texture_sets;
	};*/

	/*struct GltfMeshPrimitive
	{
		uint32_t first_index;
		uint32_t index_count;
		uint32_t vertex_count;
		GltfMaterial* material;
		GltfBoundingBox bounding_box;
	};

	struct GltfMesh
	{
		VertexBuffer vertices;
		IndexBuffer indices;

		vector<GltfMeshPrimitive> primitives;
		GltfBoundingBox bounding_box;
		GltfBoundingBox axis_aligned_bounding_box;
	};*/

	struct GltfSkin
	{
		string name;
		uint32_t skeleton_root_index;
		vector<uint32_t> joints;
		vector<matrix4F> inverse_bind_matrices;
		vector<matrix4F> joint_matrices;
	};

	/*struct GltfAnimationSampler
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
	};*/

	struct GltfNode
	{
		string name;
		uint32_t index;

		TransformF local_transform;

		vector<uint32_t> child_indices;

		GltfNode* parent_node;
		vector<GltfNode*> child_nodes;

		PbrMesh *mesh = nullptr;
		GltfSkin *skin = nullptr;
	};

	class GltfModel
	{
	public:
		GltfModel(LegacyBackend* legacy_backend, tinygltf::Model& gltfModel);
		~GltfModel();

		vector<Texture2D> textures;
		vector<PbrMaterial> materials;
		vector<PbrMesh> meshes;

		vector<GltfNode> node_storage;
		vector<GltfNode*> root_nodes;

		vector<GltfSkin> skins;
		vector<Animation> animations;

		PbrMaterial& getMaterial(uint32_t index) { return this->materials[index]; };

		float getAnimationLength(uint32_t animation_index);
		void playAnimation(uint32_t animation_index, float current_time);

		void updateSkins();

	protected:
		LegacyBackend* backend;

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

		void calcHierarchy();
	};
}