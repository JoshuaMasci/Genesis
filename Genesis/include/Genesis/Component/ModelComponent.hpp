#pragma once

#include "Genesis/Resource/Mesh.hpp"
#include "Genesis/Resource/Material.hpp"

namespace Genesis
{
	struct ModelComponent
	{
		ModelComponent() {};
		ModelComponent(shared_ptr<Mesh> mesh, shared_ptr<Material> material) :mesh(mesh), material(material) {};
		shared_ptr<Mesh> mesh;
		shared_ptr<Material> material;
	};
}