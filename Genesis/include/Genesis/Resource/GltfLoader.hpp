#pragma once

//LegacyBackend only for now
#include "Genesis/LegacyBackend/LegacyBackend.hpp"

#include "Genesis/Resource/PbrMaterial.hpp"
#include "Genesis/Resource/PbrMesh.hpp"

#include "Genesis/Resource/Material.hpp"
#include "Genesis/Resource/Mesh.hpp"

#include "Genesis/Resource/Animation.hpp"
#include "Genesis/Resource/Skeleton.hpp"

#include "tiny_gltf.h"

namespace Genesis
{
	struct GltfNode;

	struct GltfBoundingBox
	{
		vector3F min;
		vector3F max;
		GltfBoundingBox() {};
		GltfBoundingBox(vector3F min, vector3F max) : min(min), max(max) {};
	};

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
		vector<Mesh> meshes2;

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