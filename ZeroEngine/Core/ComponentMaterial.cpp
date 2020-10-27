#include "Application.h"
#include "ModuleTextures.h"

#include "ComponentMaterial.h"

ComponentMaterial::ComponentMaterial(GameObject* parent, const char* path) : Component(parent, ComponentType::MATERIAL){

	//Save path info for showing purpose and stores information about texture in TextureData structure
	texture_path = path;
	TextureData = App->textures->Load(path);

	//Retrieve mesh from gameObject and store the texture in it own texture var, later we generate texture with this info
	dynamic_cast<ComponentMesh*>(owner->GetMesh())->mesh->tex_info = TextureData;
	dynamic_cast<ComponentMesh*>(owner->GetMesh())->mesh->GenerateTextureInfo();

}

ComponentMaterial::~ComponentMaterial() {

}