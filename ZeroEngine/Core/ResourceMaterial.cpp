#include "ResourceMaterial.h"
#include "Application.h"

ResourceMaterial::ResourceMaterial(UID id) : Resource(id, ResourceType::Material) {
	
	diffuse = nullptr;
	diffuse_id = 0;

	materialColor.r = 255;
	materialColor.g = 255;
	materialColor.b = 255;
	materialColor.a = 255;
}

ResourceMaterial::~ResourceMaterial() {

}

void ResourceMaterial::SetDiffuse(ResourceTexture* source) {

	diffuse = source;
	diffuse_id = source->GetUID();
}

void ResourceMaterial::Load() {

}

void ResourceMaterial::Save() const {

}

bool ResourceMaterial::LoadInMemory() {
	return 0;
}