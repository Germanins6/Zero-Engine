#include "ResourceMesh.h"

ResourceMesh::ResourceMesh(UID id) : Resource(id, ResourceType::Mesh) {

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