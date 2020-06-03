#include "Genesis/Entity/Mesh.hpp"

namespace Genesis
{
	MeshComponent::MeshComponent(Node* node, PbrMesh* mesh)
		:Component(node)
	{
		this->mesh = mesh;
	}
}