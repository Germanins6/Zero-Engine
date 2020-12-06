#include "Application.h"
#include "ZeroImporter.h"

#include "ComponentMaterial.h"

ComponentMaterial::ComponentMaterial(GameObject* parent, UID ourMaterial) : Component(parent, ComponentType::MATERIAL){

	//Save path info for showing purpose and stores information about texture in TextureData structure
	materialReference = ourMaterial;
	

	//Check later how to adapt this block of code into UID functions to avoid create new memory for each resource 
	//Retrieve mesh from gameObject and store the texture in it own texture var, later we generate texture with this info
	/*if (owner->GetMesh() != nullptr) {
		dynamic_cast<ComponentMesh*>(owner->GetMesh())->mesh->tex_info = TextureData;
		dynamic_cast<ComponentMesh*>(owner->GetMesh())->mesh->GenerateTextureInfo();
	}*/ 


}

ComponentMaterial::~ComponentMaterial() {
}