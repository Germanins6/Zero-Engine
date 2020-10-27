#pragma once

#include "Component.h"

// -- ModuleTextures
#include "ModuleTextures.h"

class ComponentMaterial : public Component {

public:

	ComponentMaterial(GameObject* parent, const char* path);
	~ComponentMaterial();
	
	void UpdateTextureInfo(const char* path);

public:

	Texture* TextureData;
	const char* texture_path;
};