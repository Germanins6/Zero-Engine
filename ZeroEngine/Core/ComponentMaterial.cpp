#include "Application.h"
#include "ModuleTextures.h"

#include "ComponentMaterial.h"

ComponentMaterial::ComponentMaterial(GameObject* parent, const char* path) : Component(parent, ComponentType::MATERIAL){

	//Save path info for showing purpose and stores information about texture in TextureData structure
	texture_path = path;
	TextureData = App->textures->Load(path);

	//Retrieve mesh from gameObject and store the texture in it own texture var, later we generate texture with this info
	if (owner->GetMesh() != nullptr) {
		dynamic_cast<ComponentMesh*>(owner->GetMesh())->mesh->tex_info = TextureData;
		dynamic_cast<ComponentMesh*>(owner->GetMesh())->mesh->GenerateTextureInfo();
	}
}

ComponentMaterial::~ComponentMaterial() {

	glDeleteTextures(1, &TextureData->id);
	TextureData = nullptr;
	texture_path = nullptr;
}

void ComponentMaterial::UpdateTextureInfo(const char* path) {

	
	//Save path info for showing purpose and stores information about texture in TextureData structure
	this->texture_path = path;
	this->TextureData = App->textures->Load(path);

	//Retrieve mesh from gameObject and store the texture in it own texture var, later we generate texture with this info
	if (owner->GetMesh() != nullptr) {
		dynamic_cast<ComponentMesh*>(owner->GetMesh())->mesh->tex_info = TextureData;
		dynamic_cast<ComponentMesh*>(owner->GetMesh())->mesh->GenerateTextureInfo();
	}
}