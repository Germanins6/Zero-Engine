#include "Application.h"
#include "ModuleTextures.h"

#include "ComponentMaterial.h"

ComponentMaterial::ComponentMaterial(GameObject* parent, const char* path) : Component(parent, ComponentType::MATERIAL){
	
	texture_path = path;
	data = App->textures->Load(texture_path);

	dynamic_cast<ComponentMesh*>(owner->GetMesh())->mesh->tex_info = data;
	dynamic_cast<ComponentMesh*>(owner->GetMesh())->mesh->GenerateBufferTexture();
}

ComponentMaterial::~ComponentMaterial() {

}
