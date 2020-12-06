#include "ResourceMaterial.h"
#include "Application.h"

ResourceMaterial::ResourceMaterial(UID id) : Resource(id, ResourceType::Material) {
	
	if (diffuse_id != 0)
		diffuse = dynamic_cast<ResourceTexture*>(App->resources->CreateNewResource("hellonewtest", ResourceType::Texture, true, diffuse_id));
	
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