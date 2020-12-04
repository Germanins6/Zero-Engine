#include "ResourceMesh.h"

ResourceMesh::ResourceMesh(UID id) : Resource(id, ResourceType::Mesh) {

	num_index = 0;
	index = nullptr;

	num_vertex = 0;
	vertex = nullptr;

	num_normals = 0;
	normals = nullptr;

	num_uvs = 0;
	uv_coords = nullptr;

}

ResourceMesh::~ResourceMesh() {

}

void ResourceMesh::Load() {

}

void ResourceMesh::Save() const {

}

bool ResourceMesh::LoadInMemory() {
	return 0;
}