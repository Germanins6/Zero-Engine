#pragma once

#include "Component.h"

// -- ModuleTextures
#include "Textures.h"

class ComponentMaterial : public Component {

public:

	ComponentMaterial(GameObject* parent, const char* path, Texture* ourTexture);
	~ComponentMaterial();
	
	void UpdateTextureInfo(const char* path);

public:
	Texture* TextureData;
	const char* texture_path;
};