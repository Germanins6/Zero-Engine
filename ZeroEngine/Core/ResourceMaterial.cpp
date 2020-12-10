#include "ResourceMaterial.h"
#include "Application.h"

ResourceMaterial::ResourceMaterial(UID id) : Resource(id, ResourceType::Material) {
	diffuse = nullptr;
	diffuse_id = 0;
}

ResourceMaterial::~ResourceMaterial() {

}

void ResourceMaterial::Load() {

}

void ResourceMaterial::Save() const {

}

bool ResourceMaterial::LoadInMemory() {
	return 0;
}