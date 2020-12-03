#include "ResourceMaterial.h"

ResourceMaterial::ResourceMaterial(UID id) : Resource(id, ResourceType::Material) {

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